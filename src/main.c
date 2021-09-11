/**********
  * @file    main.c
  * @author  G. Garcia & A. Riedinger.
  * @version 0.1
  * @date    11.09.21
  * @brief   Indicador de Tiempo & Medidor de Temperatura sin TS.
  *
  * SALIDAS:
  	  *	LCD
  *
  * ENTRADAS:
  	  * UserButton - PC13
  	  * S1 		   - PC9
  	  * S2         - PB8
  	  * S3         - PC6
  	  * S4         - PC8
  	  * LM35 	   - PC0/ADC1
**********/

/*------------------------------------------------------------------------------
LIBRERIAS:
------------------------------------------------------------------------------*/
#include "mi_libreria.h"

/*------------------------------------------------------------------------------
DEFINICIONES:
------------------------------------------------------------------------------*/
//Tiempo de interrupcion por Systick - 50mseg:
#define TimeINT_Systick 0.05

//Pines de conexion de los pulsadores/teclado:
#define F1_Port GPIOC
#define F1 		GPIO_Pin_9
#define F2_Port GPIOB
#define F2 		GPIO_Pin_8
#define C1_Port GPIOC
#define C1 		GPIO_Pin_6
#define C2_Port GPIOC
#define C2		GPIO_Pin_8

//Pin de conexion del LM35:
#define LM35 	  GPIO_Pin_0
#define LM35_Port GPIOC

//Pin del BackLigth del DISPLAY LCD:
#define BL_PORT GPIOD
#define BL		GPIO_Pin_7

//Ticks del despachador de tareas:
#define Ticks_ClearLCD    5
#define Ticks_Switchs     2
#define Ticks_TimeIND 	  20
#define Ticks_Temperature 8

//[3] Temperatura maxima medida en grados centrigados:
#define MAXTempDegrees 206

/*------------------------------------------------------------------------------
DECLARACION DE FUNCIONES LOCALES:
------------------------------------------------------------------------------*/
void REFRESH_LCD(void);
void SWITCHS(void);
void TIME_IND(void);
void TEMPERATURE(void);

/*------------------------------------------------------------------------------
VARIABLES GLOBALES:
------------------------------------------------------------------------------*/
//Almacenamiento del valor de temperatura en grados centigrados:
float TempDegrees;

//Variables para el cronometro:
uint32_t Seg;

//Variables de temperatura:
uint32_t ContTemp = 0;

////Definicion de los pines del LCD:
LCD_2X16_t LCD_2X16[] = {
			// Name  , PORT ,   PIN      ,         CLOCK       ,   Init
			{ TLCD_RS, GPIOC, GPIO_Pin_10, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_E,  GPIOC, GPIO_Pin_11, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_D4, GPIOC, GPIO_Pin_12, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_D5, GPIOD, GPIO_Pin_2,  RCC_AHB1Periph_GPIOD, Bit_RESET },
			{ TLCD_D6, GPIOF, GPIO_Pin_6,  RCC_AHB1Periph_GPIOF, Bit_RESET },
			{ TLCD_D7, GPIOF, GPIO_Pin_7,  RCC_AHB1Periph_GPIOF, Bit_RESET }, };

int main(void)
{
/*------------------------------------------------------------------------------
VARIABLES LOCALES:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
CONFIGURACION DEL MICRO:
------------------------------------------------------------------------------*/
	SystemInit();

	//Inicializacion del DISPLAY LCD:
	INIT_LCD_2x16(LCD_2X16);

	//Inicializacion del teclado:
	INIT_DO(F1_Port, F1);
	INIT_DO(F2_Port, F2);
	INIT_DI(C1_Port, C1);
	INIT_DI(C2_Port, C2);

	//Inicializacion del LM35 como ENTRADA ANALOGICA / ADC1:
	INIT_ADC(LM35_Port, LM35);

	//[1]Inicializacion de interrupcion por tiempo cada 50 mseg:
	INIT_SYSTICK(TimeINT_Systick);

/*------------------------------------------------------------------------------
BUCLE PRINCIPAL:
------------------------------------------------------------------------------*/
    while(1)
    {

    }

}

/*------------------------------------------------------------------------------
INTERRUPCIONES:
------------------------------------------------------------------------------*/
//Interrupcion por tiempo - Systick cada 50mseg:
void SysTick_Handler()
{

}
