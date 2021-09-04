/*
 * lcd.c
 *
 *  Created on: 17 ago. 2021
 *      Author: simon
 */
#include "lcd.h"

static void write_4_bits(uint8_t value);
static void lcd_enable(void);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);

void lcd_init(void){
	//1. Configurar los pines gpio que son usados para las conexiones para el lcd
	GPIO_Handle_t lcd_signal;

	lcd_signal.pGPIOx = LCD_GPIO_PORT;
	lcd_signal.GPIO_PinConfig.GPIO_PinAltFunMode 	= GPIO_MODE_OUTPUT;
	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_RS;
	lcd_signal.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OP_TYPE_PP;
	lcd_signal.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_NO_PUPD;
	lcd_signal.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_FAST;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_RW;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_EN;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_D4;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_D5;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_D6;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber 		= LCD_GPIO_D7;
	GPIO_Init(&lcd_signal);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

	//2. Inicializar el LCD
	mdelay(40);

	/*RS = 0, para el comando LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/*RnW = 0, escribiendo al LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(0x3);

	mdelay(5);

	write_4_bits(0x3);

	udelay(150);

	write_4_bits(0x3);
	write_4_bits(0x2);

	//Funcion de establecimiento de comando
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);

	//Display ON y cursor ON
	lcd_send_command(LCD_CMD_DON_CURON);

	lcd_display_clear();

	//Entry mode set
	lcd_send_command(LCD_CMD_INCADD);
}


void lcd_send_command(uint8_t cmd){
	/*RS = 0 para el comando LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/*R/nW = 0, para Write*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(cmd >> 4);
	write_4_bits(cmd & 0x0F);
}


/*
 * Esta funcion envia un caracter al LCD
 * Aca usamos 4 bits en paralelo para la transmision de datos
 * El primer nibble alto de los datos seran enviads sobre la linea de datos D4,D5,D6,D7
 * Despues el nibble bajo sera establecido sobre la linea de datos D4,D5,D6,D7
 * */
void lcd_print_char(uint8_t data){
	/*RS = 1 para el dato de ususario LCD*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);

	/*R/nW = 0 para write*/
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(data >> 4);	/*Nibble alto*/
	write_4_bits(data & 0x0F); /*Nibble bajo*/
}

void lcd_display_clear(void){
	//Display Clear
	lcd_send_command(LCD_CMD_DIS_CLEAR);

	mdelay(2);
}

/*El cursor retorna a la posicion de inicio*/
void lcd_display_return_home(void)
{
	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);
	/*
	 * El comando de retorno debe esperar alrededor de 2ms
	 */
	mdelay(2);
}

void lcd_print_string(char *message)
{
      do
      {
          lcd_print_char((uint8_t)*message++);
      }
      while (*message != '\0');
}

/**
  *   Establece al lcd a una localizacion especifica dado por la informacion de un renglon y columna
  *   Numero de renglon (1 a 2)
  *   Numero de columna (1 a 16) asumiendo que es un display de 2x16
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
    	/*Establece el cursor a la direccion del primer renglon y al indice correspondiente*/
    	lcd_send_command((column |= 0x80));
      break;
    case 2:
    	/*Establece el cursor a la direccion del segundo renglon y al indice correspondiente*/
        lcd_send_command((column |= 0xC0));
      break;
    default:
      break;
  }
}

/*Escribe 4 bits de datos/comandos sobre las lineas D4,D5,D6,D7*/
static void write_4_bits(uint8_t value){
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, (value >> 0) & 0x1);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, (value >> 1) & 0x1);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, (value >> 2) & 0x1);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, (value >> 3) & 0x1);

	lcd_enable();
}

static void lcd_enable(void){
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
	udelay(10);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	udelay(100);	//tiempo de ejecucion > 37 us

}

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1); i++);
}

