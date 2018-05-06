#include <stdint.h>
#include<stdio.h>
#include "inc/tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "sensor_pwm.h"


xQueueHandle g_pULTRA3Queue;
#define ULTRA3_ITEM_SIZE           sizeof(uint8_t)
#define ULTRA3_QUEUE_SIZE          5
#define ULTRA3TASKSTACKSIZE        128
#define pdTRUE          ( ( BaseType_t ) 1 )



void timer1A_Init_ultra3(void);
void timer2A_Init_ultra3(void);
void timer_Enable_ultra3(void);
void Timer1A_Handler_ultra3(void);
void Timer2A_Handler_ultra3(void);

void timer5_Init(void);

void DisableInterrupts_ultra3(void);
void EnableInterrupts_ultra3(void);
void WaitForInterrupt_ultra3(void);

void GPIOPortD_Init_ultra3(void);
void GPIOPortF_Init_ultra3(void);
void GPIOPortF_Handler_ultra3(void);

char pw_para3;
uint32_t endTick_ultra3 ;
uint32_t startTick_ultra3,deltaTick_ultra3;
int time;
unsigned int count_ultra3=0;

void ultrasonic3 (void)
{

    EnableInterrupts_ultra3();
    GPIOPortF_Init_ultra3();
    GPIOPortD_Init_ultra3();

    timer1A_Init_ultra3();
    timer2A_Init_ultra3();
    timer_Enable_ultra3();
    timer5_Init();

    while(1)
    {

        WaitForInterrupt_ultra3();
        if(count_ultra3==10)
            GPIO_PORTD_DATA_R=0x40;
        else
            GPIO_PORTD_DATA_R=0x00;

    }
}




void GPIOPortD_Init_ultra3(void)
{

    SYSCTL_RCGC2_R |= 0x00000008;   /* 1) activate clock for PortB */

    GPIO_PORTD_AMSEL_R = 0x00;      /* 3) disable analog on PF */

    GPIO_PORTD_PCTL_R = 0x00000000; /* 4) PCTL GPIO on PF4-0 */

    GPIO_PORTD_DIR_R = 0xFF;        /* 5) PB0-7 as in pins */

    GPIO_PORTD_AFSEL_R = 0x00;      /* 6) disable alt funct on PF7-0 */

    GPIO_PORTD_DEN_R = 0xFF;        /* 7) enable digital I/O on PF4-0 */
}


/* multiple of millisecond delay using periodic mode */

void GPIOPortF_Init_ultra3(void)
{

    SYSCTL_RCGC2_R |= 0x00000020;   /* enable clock to GPIOF at clock gating control register */
      GPIO_PORTF_DIR_R &= ~0x08;    // (c) make PF4 in (built-in button)

      GPIO_PORTF_AFSEL_R &= ~0x08;  //     disable alt funct on PF4

      GPIO_PORTF_DEN_R |= 0x08;     //     enable digital I/O on PF4

      GPIO_PORTF_PCTL_R &= ~0x0000F000; // configure PF4 as GPIO

      GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF

      GPIO_PORTF_PUR_R |= 0x08;     //     enable weak pull-up on PF4

      GPIO_PORTF_IS_R &= ~0x08;     // (d) PF4 is edge-sensitive

      GPIO_PORTF_IBE_R |= 0x08;     //     PF4 is on both edges

      GPIO_PORTF_IEV_R &= ~0x08;    //     PF4 edge event is irelevant

      GPIO_PORTF_ICR_R = 0x08;      // (e) clear flag4

      GPIO_PORTF_IM_R |= 0x08;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***

      NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5

      NVIC_EN0_R |= 0x40000000;     // (h) enable interrupt 30 in NVIC

      EnableInterrupts_ultra3();           // (i) Clears the I bit
}

void timer1A_Init_ultra3()
{

    SYSCTL_RCGCTIMER_R |= 0x02;     /* enable clock to Timer Block 1 */

    TIMER1_CTL_R = 0;            /* disable Timer before initialization */

    TIMER1_CFG_R = 0x04;         /* 16-bit option */

    TIMER1_TAMR_R = 0x02;        /* periodic mode and down-count_ultra3er */

    TIMER1_TAILR_R = 1000;  /* Timer A interval load value register */

    TIMER1_TAPR_R = 0x01;        /* Timer A Prescale value 244(16-1) F5*/

    TIMER1_ICR_R = 0x1;          /* clear the TimerA timeout flag*/

    TIMER1_IMR_R = 0x01;         /* Enable Interrupt on Timer A timeout */

    NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFF1FFF) | 0x00007000; /*  priority 3(0111) */

    NVIC_EN0_R = 0x00200000;        /*  Enable interrupt 21 in NVIC */

}

void timer2A_Init_ultra3()
{

    SYSCTL_RCGCTIMER_R |= 0x04;     /* enable clock to Timer Block 2 */

    TIMER2_CTL_R = 0;            /* disable Timer before initialization */

    TIMER2_CFG_R = 0x04;         /* 16-bit option */

    TIMER2_TAMR_R = 0x02;        /* periodic mode and down-count_ultra3er */

    TIMER2_TAILR_R = 65536 - 1;  /* Timer A interval load value register */

    TIMER2_TAPR_R = 0x18;        /* Timer A Prescale value 24(16-1) */

    TIMER2_ICR_R = 0x1;          /* clear the TimerA timeout flag*/

    TIMER2_IMR_R = 0x01;         /* Enable Interrupt onS Timer A timeout */

    NVIC_PRI5_R = (NVIC_PRI5_R & 0x1FFFFFFF) | 0xD0000000; /*  priority 5(1011) */

    NVIC_EN0_R = 0x00800000;        /*  Enable interrupt 23 in NVIC */

}

void timer_Enable_ultra3(void)
{
    /*GPIO_PORTF_DATA_R = 4;
    GPIO_PORTF_DATA_R = 4;
    GPIO_PORTF_DATA_R = 4;
    GPIO_PORTF_DATA_R = 4;*/

    //TIMER1_CTL_R |= 0x01;        /* enable Timer A after initialization */

    TIMER2_CTL_R |= 0x01;        /* enable Timer A after initialization */

    //GPIO_PORTF_DATA_R = 0;

}

void Timer1A_Handler_ultra3(void)  //faster
{
   // GPIO_PORTF_DATA_R ^= 2;      /* turn on red LED */
    //GPIO_PORTB_DATA_R = 0x00;
    //count_ultra3=count_ultra3
    //GPIO_PORTB_DATA_R=0x00;

    TIMER1_ICR_R = 0x1;          /* clear the TimerA timeout flag*/

    count_ultra3=count_ultra3+1;





    //GPIO_PORTF_DATA_R = 0;      /* turn on red LED */

}

void Timer2A_Handler_ultra3(void)  //slower
{
    //GPIO_PORTF_DATA_R &= 0x08;

    TIMER1_CTL_R |= 0x01;        /* enable Timer A after initialization */

    //GPIO_PORTF_DATA_R ^= 8;      /* turn on GREEN LED */

    TIMER2_ICR_R = 0x1;          /* clear the TimerA timeout flag*/


    count_ultra3=0;

    TIMER5_TAILR_R = 4999999;       // prepare new 100 ms interval
    startTick_ultra3=4999999;
  TIMER5_CTL_R |= 0x00000001;     // enable

    //GPIO_PORTF_DATA_R = 0;      /* turn on red LED */
}


void timer5_Init(void)
{
      SYSCTL_RCGCTIMER_R |= 0x0020;   // 0) activate timer0

      //delay = SYSCTL_RCGCTIMER_R;

      TIMER5_CTL_R = 0x00000000;    // 1) disable timer2A during setup

      TIMER5_CFG_R = 0x00000000;    // 2) configure for 32-bit mode

      TIMER5_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count_ultra3 settings

      TIMER5_TAILR_R = 4999999;     // 4) reload value

      TIMER5_TAPR_R = 0;            // 5) bus clock resolution

      TIMER5_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag

      TIMER5_IMR_R = 0x00000001;    // 7) arm timeout interrupt

      NVIC_PRI17_R = (NVIC_PRI17_R&0xFF1FFFFF)|0x00800000; // 8) priority 4

      // interrupts enabled in the main program after all devices initialized

      // vector number 39, interrupt number 23

      NVIC_EN2_R = 0x00000040;           // 9) enable IRQ 23 in NVIC

      //TIMER0_CTL_R = 0x00000001;    // 10) enable timer0A
}


void GPIOPortF_Handler_ultra3(void)
{



        endTick_ultra3 = TIMER5_TAR_R;         // read current timer value (end)

          TIMER5_CTL_R &= ~0x00000001;    // stop timer
          GPIO_PORTF_ICR_R = 0x10;          // acknowledge flag for PF4
          deltaTick_ultra3 = startTick_ultra3- endTick_ultra3 -30000 ;

          printf("\n pulse width %ld\n",deltaTick_ultra3);

      if(deltaTick_ultra3<40000)
        pw_para3='1';
      else
        pw_para3='0';



}

/*********** DisableInterrupts_ultra3 ***************
* disable interrupts
* inputs:  none
* outputs: none
*/

void DisableInterrupts_ultra3(void)
{
    __asm ("    CPSID  I\n");
}



/*********** EnableInterrupts_ultra3 ***************
* emable interrupts
* inputs:  none
* outputs: none
*/

void EnableInterrupts_ultra3(void)
{
    __asm  ("    CPSIE  I\n");
}



/*********** WaitForInterrupt_ultra3 *************************
* go to low power mode while waiting for the next interrupt
* inputs:  none
* outputs: none
*/

void WaitForInterrupt_ultra3(void)
{
    __asm  ("    WFI\n");
}


uint32_t ULTRASONIC3Init(void)
{

    //
    // Print the current loggling LED and frequency.
    //
    UARTprintf("initialising sensor pwm\n");

    //
    // Create a queue for sending messages to the LED task.
    //
    g_pULTRA3Queue = xQueueCreate(ULTRA3_QUEUE_SIZE,ULTRA3_ITEM_SIZE);

    //
    // Create the LED task.
    //
    if( xTaskCreate(ultrasonic3, (const portCHAR *)"ULTRA3", ULTRA3TASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_ULTRA3_TASK, NULL) != pdTRUE) {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}

