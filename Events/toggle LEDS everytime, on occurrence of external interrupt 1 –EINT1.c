#include <RTL.h> 
#include <LPC21xx.h> 
#include <stdio.h> OS_TID tsk1; 
OS_RESULT RE1; int i; 

__task void job1(void); 
void EINT1_ISR(void)__irq { 
  isr_evt_set(0x0001, tsk1); 
  EXTINT = 0x02; VICVectAddr = 0; 
  } 

int main(void) { 
  os_sys_init(job1); 
} 

__task void job1(void) { 
  os_tsk_prio_self(2); 
  tsk1 = os_tsk_self(); // LED HARDWARE INITIALIZATION 
  IO0DIR |= 0x000F0000; 
  IO0CLR = 0x000F0000; // EINT1 PIN CONFIGURATION (P0.3) 
  PINSEL0 = (PINSEL0 & ~0x000000C0) | 0x000000C0; // EINT1 INTERRUPT CONFIGURATION 
  EXTMODE = 0x02; EXTPOLAR = 0x00; 
  EXTINT = 0x02; 
  VICIntSelect &= ~(1 << 15); 
  VICVectAddr0 = (int)EINT1_ISR; 
  VICVectCntl0 = 0x20 | 15; 
  VICIntEnable = (1 << 15); 
  i = 0; 
  while(1) {
    RE1 = os_evt_wait_and(0x0001, 0xFFFF); 
    if(RE1 == OS_R_EVT) { 
      if(i == 0) { 
        IO0SET = 0x000F0000; i = 1;             
        } 
      else { 
        IO0CLR = 0x000F0000; i = 0;             
      }         
    }     
  } 
} 
