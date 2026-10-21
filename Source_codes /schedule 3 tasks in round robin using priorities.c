#include <LPC21xx.h>
#include "rtl.h"

#define LED (1U << 10)

volatile char received_char;
volatile unsigned int data_ready = 0;

__task void task1(void);
__task void task2(void);
__task void task3(void);
__task void init_task(void);

void UART_Init(void)
{
    PINSEL0 = (PINSEL0 & ~0x000F000F) | 0x00050005;

    U0LCR = 0x83;
    U0DLL = 98;
    U0DLM = 0;
    U0LCR = 0x03;
    U0FCR = 0x07;

    U1LCR = 0x83;
    U1DLL = 98;
    U1DLM = 0;
    U1LCR = 0x03;
    U1FCR = 0x07;
}

char UART0_GetChar(void)
{
    while (!(U0LSR & 0x01));
    return U0RBR;
}

void UART1_PutChar(char c)
{
    while (!(U1LSR & 0x20));
    U1THR = c;
}

__task void task1(void)
{
    while (1)
    {
        if (!data_ready)
        {
            received_char = UART0_GetChar();
            data_ready = 1;
        }

        os_dly_wait(1);
    }
}

__task void task2(void)
{
    while (1)
    {
        if (data_ready)
        {
            UART1_PutChar(received_char);
            data_ready = 0;
        }

        os_dly_wait(1);
    }
}

__task void task3(void)
{
    while (1)
    {
        IO0SET = LED;
        os_dly_wait(50);

        IO0CLR = LED;
        os_dly_wait(50);
    }
}

__task void init_task(void)
{
    IO0DIR |= LED;
    IO0CLR = LED;

    UART_Init();

    os_tsk_create(task1, 1);
    os_tsk_create(task2, 1);
    os_tsk_create(task3, 1);

    os_tsk_delete_self();
}

int main(void)
{
    os_sys_init(init_task);
    while (1);
}
