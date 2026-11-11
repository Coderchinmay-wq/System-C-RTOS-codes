#include <LPC21xx.h>
#include "rtl.h"

#define ROWS   0x000F0000U
#define COLS   0x00F00000U
#define BUZZER (1U << 11)

volatile char key_value;
volatile unsigned int key_ready = 0;

const char keymap[4][4] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

__task void task1(void);
__task void task2(void);
__task void task3(void);
__task void init_task(void);

void LCD_Init(void);
void LCD_Data(char data);

char Keypad_GetKey(void)
{
    unsigned int row, col, pin;

    while (1)
    {
        IO1SET = ROWS;

        for (row = 0; row < 4; row++)
        {
            IO1SET = ROWS;
            IO1CLR = (1U << (16 + row));

            os_dly_wait(1);
            pin = IO1PIN;

            for (col = 0; col < 4; col++)
            {
                if (!(pin & (1U << (20 + col))))
                {
                    os_dly_wait(2);

                    if (!(IO1PIN & (1U << (20 + col))))
                    {
                        char key = keymap[row][col];

                        while (!(IO1PIN & (1U << (20 + col))))
                            os_dly_wait(1);

                        IO1SET = ROWS;
                        return key;
                    }
                }
            }
        }
    }
}

__task void task1(void)
{
    while (1)
    {
        if (!key_ready)
        {
            key_value = Keypad_GetKey();
            key_ready = 1;
        }

        os_dly_wait(1);
    }
}

__task void task2(void)
{
    while (1)
    {
        if (key_ready)
        {
            LCD_Data(key_value);
            key_ready = 0;
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
    PINSEL2 = 0;

    IO1DIR = (IO1DIR & ~COLS) | ROWS;
    IO1SET = ROWS;

    IO0DIR |= BUZZER;
    IO0CLR = BUZZER;

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
