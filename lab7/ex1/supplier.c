#include "common.h"

int take_pizza(table_t *table);

int main()
{
    printf("Supplier %d\n", getpid());
    int sem_id = get_sem(get_key(getenv("HOME"), PROJ_ID));
    int shm_table_id = get_shm(get_key(TABLE_NAME, TABLE_ID), SEG_SIZE);

    table_t *table = attach_shm(shm_table_id);

    while (1)
    {
        lock(sem_id, TABLE_SEM);
        if (table->pizza_count > 0)
        {
            int pizza_type = take_pizza(table);
            printf("> (pid: %d timestamp: %s) [D] Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), timestamp(), pizza_type, table->pizza_count);
            unlock(sem_id, TABLE_SEM);

            sleep(DELIVERY_TIME);
            printf("> (pid: %d timestamp: %s) [D] Dostarczam pizze: %d\n", getpid(), timestamp(), pizza_type);
            sleep(RETURN_TIME);
        }
        else
        {
            unlock(sem_id, TABLE_SEM);
        }
    }
    return 0;
}
int take_pizza(table_t *table)
{
    int pizza_type = table->array[table->to_deliver_index];
    table->array[table->to_deliver_index] = -1;
    table->to_deliver_index++;
    table->to_deliver_index %= TABLE_SIZE;
    table->pizza_count--;
    return pizza_type;
}