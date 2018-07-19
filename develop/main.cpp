#include "mbed.h"
#include "config.h"
#include "cmd.h"
#include "log.h"
#include "SNTPClient.h"
#include "stm32f4xx.h"
#include "MQTTClient.h"
#include "iot_app.h"
#include "drivers.h"


#define WulianOS "System"
BetterSerial * init_default_serial()
{
#if 0
// this is demo board from other vendor
#define TXPIN     PA_9 
#define RXPIN     PA_10 
#else
// this is our board
//#define TXPIN     PA_2 
//#define RXPIN     PA_3 
#endif 
    static BetterSerial pc(USB2UART_TX, USB2UART_RX ); 
    pc.baud(115200);


    return &pc;
}


int init_modules()
{


    BetterSerial * pc = init_default_serial();
    console_register_serial(pc,(char*) "  *** Welcome to WuLian OS ***"); 

    /******init serial and console first*****/
	
    init_log_module(pc);

    init_cmd_module();

    init_hw_module();

    init_ntp_module();
    wait(0.01);
	
    init_mqtt_module();
    wait(0.01);
	 
    init_iot_module();
    
    return 0;
}

void Setup(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
 
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
 
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
    
    SystemCoreClockUpdate();
 //   SystemCoreClock = 168000000;
    
 //HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_1);        // output SYSCLOCK to pin PC9 to monitor frequency
};

void SystemReset(void)
{

// Use DMB to wait until all outstanding
// memory accesses are completed
__DMB();
// Read back PRIGROUP and merge with SYSRESETREQ
SCB->AIRCR = 0x05FA0004 | (SCB->AIRCR & 0x700);
while(1); // Wait until reset happen

}

#if 0 
#define LED_DEMO LED1
#else
#define LED_ENABLE PD_3 
#define LED_DEMO PD_10 
#define RFIDPWR PD_6
#define CELLPWR PD_8  


#endif

void led_demo_loop()
{uint32_t sysclock,cnt;
    DigitalOut leden(LED_ENABLE);
    DigitalOut myled(LED_DEMO);
    DigitalOut rfidipower(RFIDPWR);
    DigitalOut cellpower(CELLPWR);
    leden = 1;
    rfidipower = 1;
    cellpower = 1;
    while (1) {
	myled = 1;
        wait(1);
        myled = 0;
        wait(1);
/*		
	cnt++;
	if ( cnt == 10 )
		{
			SystemReset();
			cnt = 0;
	}
*/	
	//	   sysclock = HAL_RCC_GetSysClockFreq();   
   	// LOG_RT("System Clock is %d ", sysclock);
    }
}
int main() {

	
   Setup();
    wait(0.5);  // wait for characters to finish transmitting

    
    init_modules();
    
    led_demo_loop(); 

	
}
