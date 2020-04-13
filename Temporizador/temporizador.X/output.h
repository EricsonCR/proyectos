/* 
 * File:   output.h
 * Author: Ericson
 *
 * Created on 6 de abril de 2020, 09:13 PM
 */

#ifndef OUTPUT_H
#define	OUTPUT_H

#define led         LATAbits.LATA0
#define led_tris    TRISAbits.TRISA0
#define relay       LATAbits.LATA4
#define relay_tris  TRISAbits.TRISA4

void output_config(void) {
    led = 0;
    relay = 0;
    led_tris = 0;
    relay_tris = 0;
}

void output_led(int d) {
    led = d;
}

void output_relay(int d) {
    relay = d;
}

#endif	/* OUTPUT_H */

