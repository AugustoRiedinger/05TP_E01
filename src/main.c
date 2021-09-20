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

//Parámetros de configuración del TIM3:
#define Freq 	 0.01
#define TimeBase 200e3

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

	//Inicializacion User LED de prueba como salida digital:
	INIT_DO(GPIOB, GPIO_Pin_0);
	INIT_DO(GPIOB, GPIO_Pin_7);

	//Inicializacion del DISPLAY LCD:
	INIT_LCD_2x16(LCD_2X16);


	//Inicializacion de la interrupcion por pulso externo en los pines de lecura del teclado:
	INIT_EXTINT(C1_Port, C1);
	INIT_EXTINT(C2_Port, C2);
	//Inicializacion de los pines de escirura del teclado como salidas digitales:
	INIT_DO(F1_Port, F1);
	INIT_DO(F2_Port, F2);
	//Se setea F1 y F2 para medir constantemente C2 y C1 con el INT_Handler:
	GPIO_SetBits(F1_Port, F1);
	GPIO_SetBits(F2_Port, F2);

	//Inicializacion del LM35 como ENTRADA ANALOGICA / ADC1:
	INIT_ADC(LM35_Port, LM35);

	//Inicializacion de interrupcion por tiempo cada 50 mseg:
	INIT_SYSTICK(TimeINT_Systick);

	//Inicialización del TIM3:
	INIT_TIM3();
	SET_TIM3(TimeBase, Freq);

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

//Interrupcion al vencimiento de cuenta de TIM3:
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

		//Buffers para mostrar valores de variables:
		char BufferTemperature[BufferLength];
		char BufferSeg[BufferLength];
		char BufferCont[BufferLength];

		//Refresco del LCD:
		CLEAR_LCD_2x16(LCD_2X16);

		//Mostrar temperatura:
		PRINT_LCD_2x16(LCD_2X16, 0, 0, "TDII T:");
		sprintf(BufferTemperature, "%.1f", TempDegrees);
		PRINT_LCD_2x16(LCD_2X16, 8, 0, BufferTemperature);
		PRINT_LCD_2x16(LCD_2X16, 13, 0, "^C");

		//Mostrar segundos:
		PRINT_LCD_2x16(LCD_2X16, 0, 1, "Seg:");
		sprintf(BufferSeg, "%d", Seg);
		if (Seg < 10) {
			PRINT_LCD_2x16(LCD_2X16, 5, 1, "0");
			PRINT_LCD_2x16(LCD_2X16, 6, 1, BufferSeg);
		} else
			PRINT_LCD_2x16(LCD_2X16, 5, 1, BufferSeg);

		//Mostrar indicador de pulsaciones:
		PRINT_LCD_2x16(LCD_2X16, 9, 1, "ind:");
		sprintf(BufferCont, "%d", 1);
//		if(Cont < 10)
//		{
//			PRINT_LCD_2x16(LCD_2X16, 14, 1, "0");
//			PRINT_LCD_2x16(LCD_2X16, 15, 1, BufferCont);
//		}
//		else
//			PRINT_LCD_2x16(LCD_2X16, 14, 1, BufferCont);
	}
}

//Interrupcion al pulso:
void EXTI9_5_IRQHandler(void)
{
  //Si la interrupcion fue por linea 6 (PC6):
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
	GPIO_ToggleBits(GPIOB, GPIO_Pin_7);

    //Clear the EXTI line 6 pending bit:
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
  //Si la interrupcion fue por linea 8 (PC6):
  else if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
	GPIO_ToggleBits(GPIOB, GPIO_Pin_0);

    //Clear the EXTI line 8 pending bit:
    EXTI_ClearITPendingBit(EXTI_Line8);
  }
}
