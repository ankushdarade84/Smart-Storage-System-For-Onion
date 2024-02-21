
   
/**
 * C library for the ESP8266 WiFi module with a PIC microcontroller
 * Copyright (C) 2015 Camil Staps <info@camilstaps.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *****************************************************************************
 *ORIGINALLY CREATED AS
 * File:    esp8266.h
 * Author:  Camil Staps <info@camilstaps.nl>
 * Website: http://github.com/camilstaps/ESP8266_PIC
 * Version: 0.1
 * 
 * MODFIED AND IMPROVED BY
 * File:    esp8266_functions.h
 * Author:  Aswinth Raj B  <mailtoaswinth@gmail.com>
 * Website: circuitdigest.com
 * Version: 0.1
 *
 * See:   https://circuitdigest.com/microcontroller-projects/how-to-send-email-using-pic16f877a-esp8266 for more explanation
 *
 * This is the header file for the ESP8266 PIC16F877A library where ESP runs on 115200 baudrate. 
 *
 */



#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"


#define ESP8266_STATION 0x01
#define ESP8266_SOFTAP 0x02

#define ESP8266_TCP 1
#define ESP8266_UDP 0

#define ESP8266_OK 1
#define ESP8266_READY 2
#define ESP8266_FAIL 3
#define ESP8266_NOCHANGE 4
#define ESP8266_LINKED 5
#define ESP8266_UNLINK 6

int len;



///***______________________ESP module Function Declarations__________________**///


/** Function prototypes **/
int esp8266_isStarted(void);        // Check if the module is started (AT)
int esp8266_restart(void);          // Restart module (AT+RST)
void esp8266_echoCmds(bool);        // Enabled/disable command echoing (ATE)
// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char);
// Connect to AP (AT+CWJAP)
void esp8266_connect(unsigned char*, unsigned char*);
// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void);
// Local IP (AT+CIFSR)
void esp8266_ip(unsigned char*);
// Create connection (AT+CIPSTART)
void esp8266_putch(unsigned char);
unsigned char esp8266_getch(void);
int esp8266_start(unsigned char protocol, char* ip, unsigned char port);
// Send data (AT+CIPSEND)
int esp8266_send(unsigned char*);
// Receive data (+IPD)
void esp8266_receive(unsigned char*, uint16_t, bool);
/** Functions for internal use only **/
// Print a string to the output
void esp8266_print(unsigned const char *);
// Wait for a certain string on the input
inline uint16_t _esp8266_waitFor(unsigned char *);
// Wait for any response on the input
inline unsigned char _esp8266_waitResponse(void);
void _esp8266_login_mail(unsigned char*, unsigned char*);
void _esp8266_mail_sendID(unsigned char*);
void _esp8266_mail_recID(unsigned char*);
void _esp8266_mail_subject(unsigned char*);
void _esp8266_mail_body(unsigned char*);
//********__________________End of Function Declaration_________________********///






//***Initialize UART for ESP8266**//
void Initialize_ESP8266(void)
{
    //****Setting I/O pins for UART****//
    TRISC6 = 0; // TX Pin set as output
    TRISC7 = 1; // RX Pin set as input
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required 
    baud rate and set BRGH for fast baud_rate**/
    SPBRG =77;
    SPBRGH =0;
    BRGH  = 1;  // for high baud_rate

    //_________End of baud_rate setting_________//
    
    //****Enable Asynchronous serial port*******//
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins
    //_____Asynchronous serial port enabled_______//
    //**Lets prepare for transmission & reception**//
    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception
    //__UART module up and ready for transmission and reception__//
    
    //**Select 8-bit mode**//  
    TX9   = 0;    // 8-bit reception selected
    RX9   = 0;    // 8-bit reception mode selected
    //__8-bit mode selected__//     
}
//________UART module Initialized__________//
//**Function to send one byte of date to UART**//
void esp8266_putch(unsigned char bt)  
{
    while(! TXIF);  // hold the program till TX buffer is free
    TXREG = bt; //Load the transmitter buffer with the received value
}
//_____________End of function________________//



void esp8266_print(unsigned const char *ptr) {
    while (*ptr != 0) {
        esp8266_putch(*ptr++);
    }
}
inline unsigned char esp8266_waitResponse(void) {
    unsigned char so_far[6] = {0,0,0,0,0,0};
    unsigned const char lengths[6] = {2,5,4,9,6,6};
    unsigned const char* strings[6] = {"OK", "ready", "FAIL", "no change", "Linked", "Unlink"};
    unsigned const char responses[6] = {ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE, ESP8266_LINKED, ESP8266_UNLINK};
    unsigned char received;
    unsigned char response;
    bool continue_loop = true;
    while (continue_loop) {
        received = esp8266_getch();
        for (unsigned char i = 0; i < 6; i++) {
            if (strings[i][so_far[i]] == received) {
                so_far[i]++;
                if (so_far[i] == lengths[i]) {
                    response = responses[i];
                    continue_loop = false;
                }
            } else {
                so_far[i] = 0;
            }
        }
    }
    return response;
}
 
inline uint16_t esp8266_waitFor(unsigned char *string) {
    unsigned char so_far = 0;
    unsigned char received;
    uint16_t counter = 0;
    do {
        received = esp8266_getch();
        counter++;
        if (received == string[so_far]) {
            so_far++;
        } else {
            so_far = 0;
        }
    } while (string[so_far] != 0);
    return counter;
}
 
void esp8266_ip(unsigned char* store_in) {
    esp8266_print("AT+CIFSR\r\n");
    unsigned char received;
    do {
        received = esp8266_getch();
    } while (received < '0' || received > '9');
    for (unsigned char i = 0; i < 4; i++) {
        store_in[i] = 0;
        do {
            store_in[i] = 10 * store_in[i] + received - '0';
            received = esp8266_getch();
        } while (received >= '0' && received <= '9');
        received = esp8266_getch();
    }
    esp8266_waitFor("OK");
}
//**Function to get one byte of date from UART**//
unsigned char esp8266_getch()   
{
    if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
    
    while(!RCIF);  // hold the program till RX buffer is free
    
    return RCREG; //receive the value and send it to main function
}
//_____________End of function________________//
 



//**Function to convert string to byte**//
void ESP8266_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        esp8266_putch(*st_pt++); //process it as a byte data
}
//___________End of function______________//
//**End of modified Codes**//



//**Function to configure soft_AP**//
unsigned char esp8266_config_softAP(unsigned char* softssid, unsigned char* softpass) {
    esp8266_print("AT+CWSAP=\"");
    esp8266_print(softssid);
    esp8266_print("\",\"");
    esp8266_print(softpass);
    esp8266_print("\",5,3\r\n");
    return esp8266_waitResponse();
}
//___________End of function______________//




//**Function to stations IP/MAC**//
 void esp8266_get_stationIP()
    {
    char rex;
    ESP8266_send_string("AT+CWLIF\r\n");
    LCD_String_xy(1,1,"IP:");
    do
    {
    rex = esp8266_getch() ;
    LCD_Char(rex);
    }while(rex!=',');

    LCD_String_xy(2,1,"MAC:");
    do
    {
    rex = esp8266_getch() ;
    LCD_Char(rex);
    }while(rex!='O');
    }
 //___________End of function______________//
 

 
 //**Function to enable multiple connections**//
 void _esp8266_enale_MUX()
 {
 esp8266_print("AT+CIPMUX=1\r\n"); //Enable Multiple Connections
 esp8266_waitResponse();
 }
  //___________End of function______________//
 
 
  //**Function to create server on Port 80**//
 void _esp8266_create_server()
 {
    esp8266_print("AT+CIPSERVER=1,80\r\n"); //Enable Server on port 80
    esp8266_waitResponse(); 
 }
  //___________End of function______________//
 
 
 
 /*Enter into Start typing the mail*/
 void _esp8266_start_mail()
 {
    esp8266_print("AT+CIPSEND=4,6\r\n");
    esp8266_waitResponse();
    esp8266_print("DATA\r\n");
    esp8266_waitResponse();
 }
 /*Entered into the typing mode*/
 
 
 /*End the Mail using a "."*/
 void _esp8266_End_mail()
 {
    esp8266_print("AT+CIPSEND=4,3\r\n");
    esp8266_waitResponse();
    esp8266_print(".\r\n");
    esp8266_waitResponse();
 }
 /*End of mail*/
 
 /*Quit Connection from SMPT server*/
 void _esp8266_disconnect_SMPT2GO()
 {
    esp8266_print("AT+CIPSEND=4,6\r\n");
    esp8266_waitResponse();
    esp8266_print("QUIT\r\n");
    esp8266_waitResponse();
 }
 /*Disconnected*/
 
 
 

 /*Connect to SMPT2GO server*/
 void _esp8266_connect_SMPT2GO()
 {
    esp8266_print("AT+CIPSTART=4,\"TCP\",\"mail.smtp2go.com\",2525\r\n");
    esp8266_waitResponse();
    esp8266_print("AT+CIPSEND=4,20\r\n");
    esp8266_waitResponse();
    esp8266_print("EHLO 192.168.1.123\r\n");
    esp8266_waitResponse();
    esp8266_print("AT+CIPSEND=4,12\r\n");
    esp8266_waitResponse();
    esp8266_print("AUTH LOGIN\r\n");
    esp8266_waitResponse();
 }
    /*connected to Server*/
 
 /*LOG IN with your SMPT2GO approved mail ID*/
    /*Visit the page https://www.smtp2go.com/ and sign up using any Gmail ID
     * Once you gmail ID is SMPT2GO approved convert your mail ID and password in 64 base format
     * visit https://www.base64encode.org/ for converting 64 base format online
     * FORMAT -> _esp8266_login_mail("mailID in base 64","Password in base 64");
     * This program uses the ID-> aswinthcd@gmail.com and password -> circuitdigest as an example
     */
 
 
 
 
 
 
 
 
 
 
 
/**
 * Check if the module is started
 *
 * This sends the `AT` command to the ESP and waits until it gets a response.
 *
 * @return true if the module is started, false if something went wrong
 */
int esp8266_isStarted(void) {
    esp8266_print("AT\r\n");
    return (esp8266_waitResponse() == ESP8266_OK);
}

/**
 * Restart the module
 *
 * This sends the `AT+RST` command to the ESP and waits until there is a
 * response.
 *
 * @return true iff the module restarted properly
 */
int esp8266_restart(void) {
    esp8266_print("AT+RST\r\n");
    if (esp8266_waitResponse() != ESP8266_OK) {
        return false;
    }
    return (esp8266_waitResponse() == ESP8266_READY);
}

/**
 * Enable / disable command echoing.
 *
 * Enabling this is useful for debugging: one could sniff the TX line from the
 * ESP8266 with his computer and thus receive both commands and responses.
 *
 * This sends the ATE command to the ESP module.
 *
 * @param echo whether to enable command echoing or not
 */
void esp8266_echoCmds(bool echo) {
    esp8266_print("ATE");
    if (echo) {
        esp8266_putch('1');
    } else {
        esp8266_putch('0');
    }
    esp8266_print("\r\n");
    esp8266_waitFor("OK");
}

/**
 * Set the WiFi mode.
 *
 * ESP8266_STATION : Station mode
 * ESP8266_SOFTAP : Access point mode
 *
 * This sends the AT+CWMODE command to the ESP module.
 *
 * @param mode an ORed bitmask of ESP8266_STATION and ESP8266_SOFTAP
 */
void esp8266_mode(unsigned char mode) {
    esp8266_print("AT+CWMODE=");
    esp8266_putch(mode + '0');
    esp8266_print("\r\n");
    esp8266_waitResponse();
}

/**
 * Connect to an access point.
 *
 * This sends the AT+CWJAP command to the ESP module.
 *
 * @param ssid The SSID to connect to
 * @param pass The password of the network
 * @return an ESP status code, normally either ESP8266_OK or ESP8266_FAIL
 */
void esp8266_connect(unsigned char* ssid, unsigned char* pass) {
    esp8266_print("AT+CWJAP=\"");
    esp8266_print(ssid);
    esp8266_print("\",\"");
    esp8266_print(pass);
    esp8266_print("\"\r\n");
    esp8266_waitResponse();
}

/**
 * Disconnect from the access point.
 *
 * This sends the AT+CWQAP command to the ESP module.
 */
void esp8266_disconnect(void) {
    esp8266_print("AT+CWQAP\r\n");
    esp8266_waitFor("OK");
}

/**
 * Store the current local IPv4 address.
 *
 * This sends the AT+CIFSR command to the ESP module.
 *
 * The result will not be stored as a string but byte by byte. For example, for
 * the IP 192.168.0.1, the value of store_in will be: {0xc0, 0xa8, 0x00, 0x01}.
 *
 * @param store_in a pointer to an array of the type unsigned char[4]; this
 * array will be filled with the local IP.
 */


/**
 * Open a TCP or UDP connection.
 *
 * This sends the AT+CIPSTART command to the ESP module.
 *
 * @param protocol Either ESP8266_TCP or ESP8266_UDP
 * @param ip The IP or hostname to connect to; as a string
 * @param port The port to connect to
 *
 * @return true iff the connection is opened after this.
 */
int esp8266_start(unsigned char protocol, char* ip, unsigned char port) {
    esp8266_print("AT+CIPSTART=\"");
    if (protocol == ESP8266_TCP) {
        esp8266_print("TCP");
    } else {
        esp8266_print("UDP");
    }
    esp8266_print("\",\"");
    esp8266_print(ip);
    esp8266_print("\",");
    unsigned char port_str[5] = "\0\0\0\0";
    sprintf(port_str, "%u", port);
    esp8266_print(port_str);
    esp8266_print("\r\n");
    if (esp8266_waitResponse() != ESP8266_OK) {
        return 0;
    }
    if (esp8266_waitResponse() != ESP8266_LINKED) {
        return 0;
    }
    return 1;
}

// Send data (AT+CIPSEND)
/**
 * Send data over a connection.
 *
 * This sends the AT+CIPSEND command to the ESP module.
 *
 * @param data The data to send
 *
 * @return true iff the data was sent correctly.
 */
int esp8266_send(unsigned char* data) {
    unsigned char length_str[6] = "\0\0\0\0\0";
    sprintf(length_str, "%u", strlen(data));
    esp8266_print("AT+CIPSEND=");
    esp8266_print(length_str);
    esp8266_print("\r\n");
    while (esp8266_getch() != '>');
    esp8266_print(data);
    if (esp8266_waitResponse() == ESP8266_OK) {
        return 1;
    }
    return 0;
}

/**
 * Read a string of data that is sent to the ESP8266.
 *
 * This waits for a +IPD line from the module. If more bytes than the maximum
 * are received, the remaining bytes will be discarded.
 *
 * @param store_in a pointer to a character array to store the data in
 * @param max_length maximum amount of bytes to read in
 * @param discard_headers if set to true, we will skip until the first \r\n\r\n,
 * for HTTP this means skipping the headers.
 */
void esp8266_receive(unsigned char* store_in, uint16_t max_length, bool discard_headers) {
    esp8266_waitFor("+IPD,");
    uint16_t length = 0;
    unsigned char received = esp8266_getch();
    do {
        length = length * 10 + received - '0';
        received = esp8266_getch();
    } while (received >= '0' && received <= '9');

    if (discard_headers) {
        length -= esp8266_waitFor("\r\n\r\n");
    }

    if (length < max_length) {
        max_length = length;
    }

    /*sprintf(store_in, "%u,%u:%c%c", length, max_length, esp8266_getch(), esp8266_getch());
    return;*/

    uint16_t i;
    for (i = 0; i < max_length; i++) {
        store_in[i] = esp8266_getch();
    }
    store_in[i] = 0;
    for (; i < length; i++) {
        esp8266_getch();
    }
    esp8266_waitFor("OK");
}

/**
 * Output a string to the ESP module.
 *
 * This is a function for internal use only.
 *
 * @param ptr A pointer to the string to send.
 */


/**
 * Wait until we found a string on the input.
 *
 * Careful: this will read everything until that string (even if it's never
 * found). You may lose important data.
 *
 * @param string
 *
 * @return the number of characters read
 */




