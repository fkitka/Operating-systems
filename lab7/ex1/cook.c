#include "common.h"

void bake_pizza(oven_t *oven, int pizza_type);
int take_pizza_out(oven_t *oven);
void place_on_table(table_t *table, int pizza_type);

int main()
{
    printf("Cook %d\n", getpid());
    int sem_id = get_sem(get_key(getenv("HOME"), PROJ_ID));
    int shm_oven_id = get_shm(get_key(OVEN_NAME, OVEN_ID), SEG_SIZE);
    int shm_table_id = get_shm(get_key(TABLE_NAME, TABLE_ID), SEG_SIZE);

    oven_t *oven = attach_shm(shm_oven_id);
    table_t *table = attach_shm(shm_table_id);

    srand(getpid());
    while (1)
    {
        int pizza_type = randint(0, 9);
        printf("> (pid: %d timestamp: %s) [K] Przygotowuje pizze: %d\n", getpid(), timestamp(), pizza_type);
        sleep(PREP_TIME);

        int placed_in_oven = 0;
        while (!placed_in_oven)
        {
            lock(sem_id, OVEN_SEM);
            if (oven->pizza_count < OVEN_SIZE)
            {
                bake_pizza(oven, pizza_type);
                placed_in_oven = 1;
            }
            unlock(sem_id, OVEN_SEM);
        }
        printf("> (pid: %d timestamp: %s) [K] Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), timestamp(), pizza_type, oven->pizza_count);
        sleep(BAKING_TIME);

        int placed_on_table = 0;
        while (!placed_on_table)
        {
            lock(sem_id, TABLE_SEM);
            if (table->pizza_count < TABLE_SIZE)
            {
                lock(sem_id, OVEN_SEM);
                pizza_type = take_pizza_out(oven);
                unlock(sem_id, OVEN_SEM);
                place_on_table(table, pizza_type);
                placed_on_table = 1;
            }
            unlock(sem_id, TABLE_SEM);
        }
        printf("> (pid: %d timestamp: %s) [K] Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), timestamp(), pizza_type, oven->pizza_count, table->pizza_count);
    }
    return 0;
}

void bake_pizza(oven_t *oven, int pizza_type)
{
    oven->array[oven->to_bake_index] = pizza_type;
    oven->to_bake_index++;
    oven->to_bake_index %= OVEN_SIZE;
    oven->pizza_count++;
}

int take_pizza_out(oven_t *oven)
{
    int pizza_type = oven->array[oven->to_take_out_index];
    oven->array[oven->to_take_out_index] = -1;
    oven->to_take_out_index++;
    oven->to_take_out_index %= OVEN_SIZE;
    oven->pizza_count--;
    return pizza_type;
}
void place_on_table(table_t *table, int pizza_type)
{
    table->array[table->to_place_index] = pizza_type;
    table->to_place_index++;
    table->to_place_index %= TABLE_SIZE;
    table->pizza_count++;
}