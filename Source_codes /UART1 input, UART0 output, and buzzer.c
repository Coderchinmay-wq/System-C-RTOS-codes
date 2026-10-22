#include <LPC21xx.h>
#include "rtl.h"

#define BUZZER (1U << 11)

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

char UART1_GetChar(void)
{
    while (!(U1LSR & 0x01));
    return U1RBR;
}

void UART0_PutChar(char c)
{
    while (!(U0LSR & 0x20));
    U0THR = c;
}

__task void task1(void)
{
    while (1)
    {
        if (!data_ready)
        {
            received_char = UART1_GetChar();
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
            UART0_PutChar(received_char);
            data_ready = 0;
        }

        os_dly_wait(1);
    }
}

__task void task3(void)
{
    while (1)
    {
        IO0SET = BUZZER;
        os_dly_wait(5);

        IO0CLR = BUZZER;
        os_dly_wait(5);
    }
}

__task void init_task(void)
{
    IO0DIR |= BUZZER;
    IO0CLR = BUZZER;

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
