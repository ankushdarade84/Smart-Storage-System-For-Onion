
// PIC18F4550 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = OFF     // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON    // MCLR Pin Enable bit (RE3 input pin enabled; MCLR pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include<string.h>
#include<stdio.h>
#include <pic18f4550.h>
#include"lcd.h"
#include "esp.h"
#define DHT11_Data_Pin   PORTDbits.RD1
#define DHT11_Data_Pin_Direction  TRISDbits.RD1
#define gas_sensor TRISAbits.RA0
#define heater PORTCbits.RC1
//heater
#define fan PORTCbits.RC2
//Fan
#define gs_light PORTCbits.RC0
//gas detected light
#define loop_light PORTDbits.RD2
//loop light

#define _XTAL_FREQ 12000000 //20 Mhz

unsigned char Check_bit, Temp_byte_1, Temp_byte_2, RH_byte_1, RH_byte_2;
unsigned char Himudity, RH, Sumation, new_humidity, temperature;
float tempr = 0, humidity = 0;
void dht11_init();
void find_response();
char read_dht11(); //dht11
char temp[100] = "";
char name[10][50], full[50];
int i = 0, j = 0, k = 0;
double value = 0;
int thousand = 0, hundred = 0, tens = 0, units = 0;
unsigned int a;
char adc1[30];

    char d[60];

void main() {
    //RCONbits.IPEN = 1;
    //INTCONbits.GIE = 1;
    //IPR1bits.ADIP = 1;
    //PIE1bits.ADIE = 1;
    //PIR1bits.ADIF = 0; // A/D Conversion flag bit cleareD
    gas_sensor = 1;
    TRISCbits.RC0=0;
     TRISCbits.RC1=0;
      TRISCbits.RC2=0;
     TRISDbits.RD2=0;

    LCD_Init();
    ADCON0 = 0x02;
    ADCON1 = 0x0E;
    ADCON2 = 0x8A;

    LCD_Init(); /* Initialize 16x2 LCD */

    LCD_String_xy(1, 0, "  Smart  Onion "); /* Display string at location(row,location). */
    /* This function passes string to display */
    LCD_String_xy(2, 0, " Storage System"); /*Display string at location(row,location). */
    __delay_ms(1000);
    LCD_Clear();
    __delay_ms(50);
    TRISD = 0x00;
    LCD_Init();
    Initialize_ESP8266();

    LCD_Clear();

    /*Check if the ESP_PIC communication is successful*/
    do {

        LCD_String_xy(1, 1, "ESP not found");
    } while (!esp8266_isStarted());
        //wait till the ESP send back "OK"

    LCD_String_xy(1, 1, "ESP is connected");

    LCD_Clear();
    /*Yes ESP communication successful*/

    /*Put the module in Soft AP  mode*/
    esp8266_mode(3);

    LCD_String_xy(1, 1, "ESP set as AP");

    /*Module set as AP */
    esp8266_connect("Smart", "Smart@123");
 LCD_String_xy(1, 1, "               ");
    LCD_String_xy(1, 1, "AP configured");
    esp8266_print("AT+CIPMUX=1\r\n");
    esp8266_waitFor("OK");
     ADCON0bits.GO=1;
    ADCON0bits.ADON=1;
    LCD_String_xy(1, 0, "                ");
    LCD_String_xy(2, 0, "                ");
    while(true)
    {
        loop_light=1;
       //DHT11 INITTIALISATION STARTS 
        DHT11_Data_Pin_Direction = 0; //Configure RD0 as output
    DHT11_Data_Pin = 0; //RD0 sends 0 to the sensor
    __delay_ms(18);
    DHT11_Data_Pin = 1; //RD0 sends 1 to the sensor
    __delay_us(30);
    DHT11_Data_Pin_Direction = 1; //Configure RD0 as input
    // //DHT11 INITTIALISATION ENDS
        //DHT11 RESPPONSE STARTS 
      Check_bit = 0;
    __delay_us(40);
    if (DHT11_Data_Pin == 0) {
        __delay_us(80);
        if (DHT11_Data_Pin == 1) {
            Check_bit = 1;
        __delay_us(50);
          //DHT11 RESPPONSE ENDS
        
        if (Check_bit == 1) {
            RH_byte_1 = read_dht11();
            RH_byte_2 = read_dht11();
            Temp_byte_1 = read_dht11();
            Temp_byte_2 = read_dht11();
            Sumation = read_dht11();
            if (Sumation == ((RH_byte_1 + RH_byte_2 + Temp_byte_1 + Temp_byte_2) & 0XFF)) {
                Himudity = Temp_byte_1;
                new_humidity = Himudity;
                tempr =(float) Himudity;
                RH = RH_byte_1;
                temperature = RH;
                humidity = (float) RH;
                
                
                LCD_String_xy(1, 0, "Temp: ");
                LCD_Char(48 + ((Himudity / 10) % 10));
                LCD_Char(48 + (Himudity % 10));
                LCD_Char(0xDF);
                LCD_String_xy(1, 9, "C");
                LCD_String_xy(2, 0, "Humidity: ");
                //LCD_String_xy("                ");
                LCD_Char(48 + ((RH / 10) % 10));
                LCD_Char(48 + (RH % 10));
                LCD_String_xy(2, 12, "%    ");
            }
        } else {
            LCD_String_xy(1, 0, "Error!!!               ");
            LCD_String_xy(2, 0, "No Response.             ");
        }
        __delay_ms(1000);
        LCD_String_xy(1, 0, "                ");
        LCD_String_xy(1, 0, "AIR Q      ");
        LCD_String_xy(1, 6, "         ");
        //ADC CODE STARTS
    while(ADCON0bits.GO==1);
    a = ADRESH << 8;
    a = ADRESL | a;
    //value = a* (0.004887);
    value=a*0.09775171;
    sprintf(adc1, "%.2f", value);
    ADCON0bits.GO=1;
    //ADC CODE ENDS
        LCD_String_xy(1, 9, adc1);
         LCD_String_xy(2, 0, "                   ");
         __delay_ms(500);
        if (value>30) {
            LCD_String_xy(2, 0, "  Gas DETECTED ");
            gs_light=1;
            __delay_ms(500);
        }
        else
        {
              gs_light=0;
        }
        if (RH >= 80) {
            fan=1;
            LCD_String_xy(1, 0, "Fan On,T=");
            LCD_Char(48 + ((Himudity / 10) % 10));
            LCD_Char(48 + (Himudity % 10));
            LCD_Char(0xDF);
            LCD_String_xy(1, 12, "C ");
            heater=1;
            LCD_String_xy(2, 0, "Heater On,H=");
            LCD_Char(48 + ((RH / 10) % 10));
            LCD_Char(48 + (RH % 10));
            LCD_String_xy(2, 14, "%");
        } 
        else {

            if (Himudity >= 40) {
                heater=0;
                fan=1;
                LCD_String_xy(1, 0, "Fan On,T=");
                LCD_Char(48 + ((Himudity / 10) % 10));
                LCD_Char(48 + (Himudity % 10));
                LCD_Char(0xDF);
                LCD_String_xy(1, 12, "C");
                LCD_String_xy(2, 0, "                ");
            } else {
                if(Himudity <= 10){
                heater=1;
                LCD_String_xy(1, 0, "Heater On,T=");
                LCD_Char(48 + ((Himudity / 10) % 10));
                LCD_Char(48 + (Himudity % 10));
                LCD_Char(0xDF);
                LCD_String_xy(1, 16, "C ");
                LCD_String_xy(2, 0, "                ");
                }
                else{
                fan=0;
                heater=0;
                }
            }
        }
    }
        esp8266_print("AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",80\r\n");
        esp8266_waitFor("OK");
        esp8266_print("AT+CIPSEND=0,80\r\n");
        esp8266_waitFor(">");
       sprintf(d, "GET /update?api_key=N6Z70XIBHK6JN0DR&field1=%.2f&field2=%.2f&field3=%.2f\r\n",tempr,humidity,value);
        esp8266_print(d);
        LCD_String_xy(1, 0, "                ");
        LCD_String_xy(2, 0, "                ");
        LCD_String_xy(1, 0, "Temp: ");
        LCD_Char(48 + ((Himudity / 10) % 10));
        LCD_Char(48 + (Himudity % 10));
        LCD_Char(0xDF);
        LCD_String_xy(1, 9, "C     ");
        LCD_String_xy(2, 0, "Humidity: ");
        //LCD_String_xy("                ");
        LCD_Char(48 + ((RH / 10) % 10));
        LCD_Char(48 + (RH % 10));
        LCD_String_xy(2, 12, "%    ");
        __delay_ms(1500);
        esp8266_print("AT+CIPCLOSE=0\r\n");
        
        
        LCD_String_xy(1, 0, "                ");
        LCD_String_xy(1, 0, "AIR Q      ");
        LCD_String_xy(1, 6, "         ");
        //ADC CODE STARTS
    while(ADCON0bits.GO==1);
    a = ADRESH << 8;
    a = ADRESL | a;
    //value = a* (0.004887);
    value=a*0.09775171;
    sprintf(adc1, "%.2f", value);
    ADCON0bits.GO=1;
    //ADC CODE ENDS
        LCD_String_xy(1, 9, adc1);
         LCD_String_xy(2, 0, "                   ");
        __delay_ms(1500); 
        
        
        LCD_String_xy(1, 0, "Temp: ");
        LCD_Char(48 + ((Himudity / 10) % 10));
        LCD_Char(48 + (Himudity % 10));
        
        LCD_Char(0xDF);
        LCD_String_xy(1, 10, "C     ");
        LCD_String_xy(2, 0, "Humidity: ");
        //LCD_String_xy("                ");
        LCD_Char(48 + ((RH / 10) % 10));
        LCD_Char(48 + (RH % 10));
        LCD_String_xy(2, 12, "%    ");
        __delay_ms(1500);
       
        
        LCD_String_xy(1, 0, "                ");
        LCD_String_xy(1, 0, "AIR Q      ");
        LCD_String_xy(1, 6, "         ");
        //ADC CODE STARTS
    while(ADCON0bits.GO==1);
    a = ADRESH << 8;
    a = ADRESL | a;
    //value = a* (0.004887);
    value=a*0.09775171;
    sprintf(adc1, "%.2f", value);
    ADCON0bits.GO=1;
    //ADC CODE ENDS
        LCD_String_xy(1, 9, adc1);
         LCD_String_xy(2, 0, "                   ");
        __delay_ms(1500); 
loop_light=0;
 __delay_ms(1000); 
}
}
}

char read_dht11() {
    char data, for_count;
    for (for_count = 0; for_count < 8; for_count++) {
        while (!DHT11_Data_Pin);
        __delay_us(30);
        if (DHT11_Data_Pin == 0) {
            data &= ~(1 << (7 - for_count)); //Clear bit (7-b)
        } else {
            data |= (1 << (7 - for_count)); //Set bit (7-b)
            while (DHT11_Data_Pin);
        } //Wait until PORTD.F0 goes LOW
    }
    return data;
}
void print_temp()
{
    
}
void print_gas()
{
    
}





