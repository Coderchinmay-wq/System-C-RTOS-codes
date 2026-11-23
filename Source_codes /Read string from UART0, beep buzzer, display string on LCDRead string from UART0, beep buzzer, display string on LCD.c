#include <LPC21xx.h>
#include "rtl.h"

#define BUZZER (1U << 11)

#define EVENT_DATA 0x01
#define EVENT_BEEP 0x02

char received_string[20];

__task void task1(void);
__task void task2(void);
__task void task3(void);
__task void init_task(void);

void UART0_Init(void)
{
    PINSEL0 = (PINSEL0 & ~0x0FU) | 0x05U;

    U0LCR = 0x83;
    U0DLL = 98;
    U0DLM = 0;
    U0LCR = 0x03;
    U0FCR = 0x07;
}

char UART0_GetChar(void)
{
    while (!(U0LSR & 0x01))
        os_dly_wait(1);

    return U0RBR;
}

void LCD_Command(unsigned char cmd);
void LCD_Data(char data);

void LCD_DisplayString(char *s)
{
    unsigned int i;

    LCD_Command(0x01);
    LCD_Command(0x80);

    for (i = 0; s[i] != '\0'; i++)
        LCD_Data(s[i]);
}

__task void task1(void)
{
    unsigned int i = 0;
    char c;

    while (1)
    {
        i = 0;

        while (1)
        {
            c = UART0_GetChar();

            if (c == '\r' || c == '\n')
                break;

            if (i < 19)
                received_string[i++] = c;
        }

        received_string[i] = '\0';

        os_evt_set(EVENT_DATA, os_tsk_self());

        os_dly_wait(1);
    }
}

__task void task2(void)
{
    while (1)
    {
        os_evt_wait_or(EVENT_DATA, 0xFFFF);

        IO0SET = BUZZER;
        os_dly_wait(5);

        IO0CLR = BUZZER;
        os_dly_wait(5);

        os_evt_set(EVENT_BEEP, os_tsk_self());
    }
}

__task void task3(void)
{
    while (1)
    {
        os_evt_wait_or(EVENT_BEEP, 0xFFFF);

        LCD_DisplayString(received_string);
    }
}

__task void init_task(void)
{
    IO0DIR |= BUZZER;
    IO0CLR = BUZZER;

    UART0_Init();
    LCD_Init();

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
