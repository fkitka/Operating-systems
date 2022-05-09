#include "common.h"

int take_pizza(table_t *table);

int main()
{
    printf("Supplier %d\n", getpid());
    sem_t *table_sem = get_sem(TABLE_SEM);
    int shm_table_fd = get_shm(TABLE_NAME);

    table_t *table = attach_shm(shm_table_fd);

    while (1)
    {
        lock(table_sem);
        if (table->pizza_count > 0)
        {
            int pizza_type = take_pizza(table);
            printf("> (pid: %d timestamp: %s) [D] Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), timestamp(), pizza_type, table->pizza_count);
            unlock(table_sem);

            sleep(DELIVERY_TIME);
            printf("> (pid: %d timestamp: %s) [D] Dostarczam pizze: %d\n", getpid(), timestamp(), pizza_type);
            sleep(RETURN_TIME);
        }
        else
        {
            unlock(table_sem);
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