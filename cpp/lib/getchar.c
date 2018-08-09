#include <machine.h>

int tgt_getchar()
{
    volatile unsigned *const uart_status = (void *)UART_STATUS_ADDR;
    volatile unsigned *const uart_data = (void *)UART_DATA_ADDR;
    while (!(*uart_status & 2))
        ;
    return *uart_data;
}

int getchar()
{
    return tgt_getchar();
}