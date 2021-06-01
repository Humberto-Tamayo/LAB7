//Practica 6 LCD Equipo 1
////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
#include "device_config.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

//+++++++++++++++++++++++++++++++++++++| DIRECTIVES |+++++++++++++++++++++++++++++++++++++

#define _XTAL_FREQ 2000000
#define SWEEP_STEP 10
#define SWEEP_FREQ 10

#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0

//+++++++++++++++++++++++++++++++++++++| DATA TYPES |+++++++++++++++++++++++++++++++++++++
enum por_ACDC {digital, analog};          // digital = 0, analog = 1
enum por_dir{output, input};

////+++++++++++++++++++++++++++++++++++++| ISRs |+++++++++++++++++++++++++++++++++++++
//// ISR for high priority
//void __interrupt ( high_priority ) high_isr( void );
//// ISR for low priority
//void __interrupt ( low_priority ) low_isr( void );

//+++++++++++++++++++++++++++++++++++++| FUNCTION DECLARATIONS |+++++++++++++++++++++++++++++++++++++
void LCD_rdy(void);
void LCD_init(void);
void LCD_cmd(char cx);
void send2LCD(char xy);
char key_scanner(void);
void portsInit(void);
void Timer1_delay(void);
void Timer0_delay(void);

//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main(void){ 
    LCD_init();
    portsInit();
    
    TRISBbits.TRISB4=0;      //Port-B pin 4 as output
    
//    TRISB=0;  			/* Set as output port */
//    LATB = 0xff;  		/* Send high on PortB */
    
    int counter = 0;
    

    while(1){
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x80);
        send2LCD(0x46); // F
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x81);
        send2LCD(0x52); // R
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x82);
        send2LCD(0x45); // E
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x83);
        send2LCD(0x51); // Q
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x84);
        send2LCD(0x55); // U
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x85);
        send2LCD(0x45); // E
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x86);
        send2LCD(0x4E); // N
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x87);
        send2LCD(0x43); // C
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x88);
        send2LCD(0x59); // Y
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0x89);
        send2LCD(0x3A); // :
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0xC4);
        send2LCD(0x48); // H
        __delay_ms(SWEEP_STEP);
        LCD_cmd (0xC5);
        send2LCD(0x5A); // Z
        
        
        while (counter < 2) {
            //Timer1_delay();
            Timer0_delay();
            //__delay_ms(1);
            if (counter <= 1) {
                LCD_cmd (0xCA);
                send2LCD(0x48); // 1  
            }
            else {
                LCD_cmd (0xCA);
                send2LCD(0x5A); // 1
            }
            counter = counter + 1;
        }
        counter = 0;
    }
}

void portsInit(void) {
    ANSELA = digital;   // Set port A as Digital for keypad driving
    TRISA  = 0xF0;      // For Port A, set pins 4 to 7 as inputs (columns), and pins 0 to 3 as outputs (rows)
    //OSCCON = 0x74;      // Set the internal oscillator to 8MHz and stable
}

void Timer1_delay()
{
    /* Enable 16-bit TMR1 register, No pre-scale, internal clock,timer OFF */
    T1CON=0x80;  

    TMR1=0xf830;                /* Load count for generating delay of 1 ms */
    T1CONbits.TMR1ON=1;     	/* Turn ON Timer1 */
    while(PIR1bits.TMR1IF==0); 	/* Wait for Timer1 overflow interrupt flag */
    TMR1ON=0;               	/* Turn OFF timer */
    TMR1IF=0;                   /* Make Timer1 overflow flag to '0' */
}

void Timer0_delay()
{
    T1CON=0x01;             //Timer-1 16-bit mode Prescaler 1:4
    TMR1H=0x30;             //Count Hight Byte
    TMR1L=0xD4;             //Count Low Byte
    
    //Runing loop for 100 times produces 1 second 10ms x 100 = 1 second
    for(int i=1;i <= 100;i++){         
    T1CONbits.TMR1ON=1;              //Run timer
    while(INTCONbits.TMR0IF==0);     //Wait for flag to over flow
    T1CONbits.TMR1ON=0;              //Switch off timer
    INTCONbits.TMR0IF=0;             //Clear Interrupt
    }
}


//+++++++++++++++++++++++++++++++++++++| FUNCTIONS |+++++++++++++++++++++++++++++++++++++
// Function to wait until the LCD is not busy
void LCD_rdy(void){
    char test;
    // configure LCD data bus for input
    LCD_DATA_DIR = 0b11111111;
    test = 0x80;
    while(test){
        LCD_RS = 0;         // select IR register
        LCD_RW = 1;         // set READ mode
        LCD_E  = 1;         // setup to clock data
        test = LCD_DATA_R;
        Nop();
        LCD_E = 0;          // complete the READ cycle
        test &= 0x80;       // check BUSY FLAG 
    }
    LCD_DATA_DIR = 0b00000000;
}
//
// LCD initialization function
void LCD_init(void){
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x40
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    LCD_cmd(0x01);          // Clear display and move cursor home
    
    
    //OSCCON = 0x74;      // Set the internal oscillator to 8MHz and stable
}

// Send command to the LCD
void LCD_cmd(char cx) {
    LCD_rdy();              // wait until LCD is ready
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    Nop();
    LCD_DATA_W = cx;        // send out command
    Nop();                  // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

// Function to display data on the LCD
void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
}