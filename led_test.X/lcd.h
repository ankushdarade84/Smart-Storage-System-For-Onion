
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
  #define RS LATD6                   /* PORTD 0 pin is used for Register Select */
#define EN LATD7                   /* PORTD 1 pin is used for Enable */
#define ldata LATB                 /* PORTB is used for transmitting data to LCD */

#define LCD_Port TRISB              
#define LCD_Control1 TRISDbits.RD6
#define LCD_Control2 TRISDbits.RD7
#include <xc.h> // include processor files - each processor file is guarded.  
/*********************************Delay Function********************************/
void MSdelay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<val;i++)
            for(j=0;j<303;j++);      /*This count Provide delay of 1 ms for 8MHz Frequency */
 }
 

void LCD_Command(char cmd )
{
	ldata= cmd;            /* Send data to PORT as a command for LCD */   
	RS = 0;                /* Command Register is selected */
	EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
	NOP();
	EN = 0;
	MSdelay(3);	
    ldata= ((cmd<<4) & 0xF0);           /* Send data to PORT as a command for LCD */   
	RS = 0;                /* Command Register is selected */
	EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
	NOP();
	EN = 0;
	MSdelay(3);	
}

void LCD_Char(char dat)
{
	ldata= dat;            /* Send data to LCD */  
	RS = 1;                /* Data Register is selected */
	EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
	NOP();
	EN=0;
	MSdelay(1);
    ldata= ((dat<<4) & 0xF0);           /* Send data to LCD */  
	RS = 1;                /* Data Register is selected */
	EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
	NOP();
	EN=0;
	MSdelay(1);
}


void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    	}
		
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    LCD_String(msg);

}
void LCD_Init()
{
    MSdelay(15);           /* 15ms,16x2 LCD Power on delay */
    LCD_Port = 0x00;       /* Set PORTB as output PORT for LCD data(D0-D7) pins */
    LCD_Control1=0;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
    LCD_Control2=0;
    LCD_Command(0x02);     /* uses 2 line and initialize 5*7 matrix of LCD */
     LCD_Command(0x28); 
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}
void LCD_Clear()
{
    	LCD_Command(0x01); /* clear display screen */
}


  
// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

