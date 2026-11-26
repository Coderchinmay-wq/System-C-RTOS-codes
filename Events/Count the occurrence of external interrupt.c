#include <RTL.h> 
#include <LPC21xx.h> 
#include <stdio.h> 
OS_TID tsk1; 
OS_RESULT RE1; 
int i; 
__task void job1(void); 
void EINT0_ISR(void) __irq 
{ 
isr_evt_set(0x0001, tsk1); 
EXTINT = 0x01; 
VICVectAddr = 0; 
} 
int main(void) 
{ 
os_sys_init(job1); 
} 
__task void job1(void) 
{ 
os_tsk_prio_self(2); 
tsk1 = os_tsk_self(); 
// LED HARDWARE INITIALIZATION 
IO0DIR |= 0x000F0000; 
IO0CLR = 0x000F0000; 
// EINT0 PIN CONFIGURATION (P0.1) 
PINSEL0 = (PINSEL0 & ~0x0000000C) | 0x0000000C; 
// EINT0 INTERRUPT CONFIGURATION 
EXTMODE = 0x01; 
EXTPOLAR = 0x00; 
EXTINT = 0x01; 
VICIntSelect &= ~(1 << 14); 
VICVectAddr0 = (int)EINT0_ISR; 
VICVectCntl0 = 0x20 | 14; 
VICIntEnable = (1 << 14); 
i = 0; 
while(1) 
{ 
RE1 = os_evt_wait_and(0x0001, 0xFFFF); 
if(RE1 == OS_R_EVT) 
{ 
i = (i + 1) & 0x0F; 
IO0CLR = 0x000F0000; 
IO0SET = (i << 16); 
        } 
    } 
}
