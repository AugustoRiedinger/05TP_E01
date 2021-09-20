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
#define Freq 	 4		//Equivalente a 250mseg
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

//Ticks del despachador de tareas:
#define Ticks_ClearLCD    5
#define Ticks_Switchs     2
#define Ticks_TimeIND 	  20
#define Ticks_Temperature 8

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

//Variables del TS:
uint32_t Switchs;
uint32_t TimeIND;
uint32_t Temperature;

//Variables para el conteo de los pulsadores:
uint32_t S1Cont = 0;
uint32_t S2Cont = 0;
uint32_t S3Cont = 0;
uint32_t S4Cont = 0;
uint32_t Cont   = 0;

//Variables para el cronometro:
uint32_t Seg = 0;

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
	//Se setea F1 en 1 para que arranque en un valor logico distinto a F2:
	GPIO_SetBits(F1_Port, F1);

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
		if (Switchs == Ticks_Switchs)
			SWITCHS();
		else if(TimeIND == Ticks_TimeIND)
			TIME_IND();
		else if(Temperature == Ticks_Temperature)
			TEMPERATURE();
		else if(TimeIND == Ticks_TimeIND)
			TIME_IND();
    }

}

/*------------------------------------------------------------------------------
INTERRUPCIONES:
------------------------------------------------------------------------------*/
//Interrupcion por tiempo - Systick cada 50mseg:
void SysTick_Handler()
{
	Switchs++;
	TimeIND++;
	Temperature++;
	TimeIND++;
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
		sprintf(BufferCont, "%d", Cont);
		if (Cont < 10) {
			PRINT_LCD_2x16(LCD_2X16, 14, 1, "0");
			PRINT_LCD_2x16(LCD_2X16, 15, 1, BufferCont);
		} else
			PRINT_LCD_2x16(LCD_2X16, 14, 1, BufferCont);
	}
}

//Interrupcion al pulso por PC6-C1 o PC8-C2:
void EXTI9_5_IRQHandler(void)
{
  //Si la interrupcion fue por linea 6 (PC6 - C1):
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
	//Si ademas de estar C1 en 1 tambien esta F1 en 1, entonces el switch pulsado es S1:
	if(GPIO_ReadInputDataBit(F1_Port, F1))
		S1Cont = S1Cont + 1;
	//Si ademas de estar C1 en 1 tambien esta F2 en 1, entonces el switch pulsado es S2:
	else if(GPIO_ReadInputDataBit(F2_Port, F2))
		S2Cont = S2Cont + 2;

    //Clear the EXTI line 6 pending bit:
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
  //Si la interrupcion fue por linea 8 (PC8 - C2):
  else if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
	//Si ademas de estar C2 en 1 tambien esta F1 en 1, entonces el switch pulsado es S3:
	if (GPIO_ReadInputDataBit(F1_Port, F1))
		S3Cont = S3Cont + 3;
	//Si ademas de estar C2 en 1 tambien esta F2 en 1, entonces el switch pulsado es S4:
	else if (GPIO_ReadInputDataBit(F2_Port, F2))
		S4Cont = S4Cont + 4;

    //Clear the EXTI line 8 pending bit:
    EXTI_ClearITPendingBit(EXTI_Line8);
  }

  //Si cont llega a 100, se reseta y comienza de cero:
  if(Cont >= 100)
  	{
  		Cont = 0;
  		S1Cont = 0;
  		S2Cont = 0;
  		S3Cont = 0;
  		S4Cont = 0;
  	}
  //Sino, se actualiza el valor de cont y se agrega a la sumatoria general:
  else
		Cont = S1Cont + S2Cont + S3Cont + S4Cont;
}

/*------------------------------------------------------------------------------
TAREAS DEL TS:
------------------------------------------------------------------------------*/
//Manejo de los pulsadores:
void SWITCHS(void)
{
	//Reset variables del TS:
	Switchs = 0;

	//Se prender y apagan F1 y F2 para preguntar en el INT_Handler:
	GPIO_ToggleBits(F1_Port, F1);
	GPIO_ToggleBits(F2_Port, F2);
}

//Manejo del indicador de tiempo:
void TIME_IND(void)
{
	//Reset variables del TS:
	TimeIND = 0;

	//Si la variable de tiempo es mayor a 100, se resetea:
	if(Seg >= 99)
		Seg = 0;
	//Sino, se aumenta hasta llegar a 100.
	else
		Seg++;
}
//Manejo del la temperatura:
void TEMPERATURE(void)
{
	//Reset variables del TS:
	Temperature = 0;

	//Almacenamiento del valor de temperatura en cuentas digitales:
	uint32_t TempDig;

	//Lectura del valor de temperatura digital:
	TempDig = READ_ADC(LM35_Port, LM35);

	//[4] Conversion de valor digital a grados centigrados:
	ContTemp++;
	if (ContTemp == 5) {
		TempDegrees = (float) TempDig * MAXTempDegrees / 4095;
		ContTemp = 0;
	}
}
