
#include <LPC21xx.h>
#include "rtl.h"

#define BUZZER (1U << 11)
#define MOTOR  (1U << 12)

#define SEGMENTS (0xFFU << 16)
#define DIGIT1   (1U << 24)
#define DIGIT2   (1U << 25)

const unsigned char seg_code[10] =
{
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

__task void task1(void);
__task void task2(void);
__task void task3(void);
__task void init_task(void);

void DisplayDigit(unsigned int digit,
                  unsigned int position)
{
    IO1CLR = SEGMENTS | DIGIT1 | DIGIT2;

    IO1SET = ((unsigned int)seg_code[digit] << 16);

    if (position == 1)
        IO1SET = DIGIT1;
    else
        IO1SET = DIGIT2;

    os_dly_wait(2);

    IO1CLR = DIGIT1 | DIGIT2;
}

__task void task1(void)
{
    unsigned int number = 0;
    unsigned int i;

    while (1)
    {
        for (i = 0; i < 30; i++)
        {
            DisplayDigit(number / 10, 1);
            DisplayDigit(number % 10, 2);
        }

        number++;

        if (number > 99)
            number = 0;
    }
}

__task void task2(void)
{
    while (1)
    {
        IO0SET = BUZZER;
        os_dly_wait(5);

        IO0CLR = BUZZER;
        os_dly_wait(5);
    }
}

__task void task3(void)
{
    while (1)
    {
        IO0SET = MOTOR;
        os_dly_wait(10);
    }
}

__task void init_task(void)
{
    PINSEL2 = 0x00000000;

    IO1DIR |= SEGMENTS | DIGIT1 | DIGIT2;
    IO1CLR = SEGMENTS | DIGIT1 | DIGIT2;

    IO0DIR |= BUZZER | MOTOR;
    IO0CLR = BUZZER | MOTOR;

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
