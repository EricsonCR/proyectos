/* 
 * File:   input.h
 * Author: Ericson
 *
 * Created on 6 de abril de 2020, 07:24 PM
 */

#ifndef INPUT_H
#define	INPUT_H

#define opto            PORTAbits.RA5
#define opto_tris       TRISAbits.TRISA5
#define pulsador_subir  PORTAbits.RA3
#define subir_tris      TRISAbits.TRISA3
#define pulsador_bajar  PORTCbits.RC5
#define bajar_tris      TRISCbits.TRISC5
#define pulsador_ok     PORTCbits.RC4
#define ok_tris         TRISCbits.TRISC4

void input_config(void) {
    opto_tris = 1;
    subir_tris = 1;
    bajar_tris = 1;
    ok_tris = 1;
}

unsigned int input_pulsador_subir(void) {
    static int contador = 0;
    if (pulsador_subir) {
        if (++contador > 10000) {
            contador = 0;
            return 1;
        }
    } else {
        contador = 0;
        return 0;
    }
    return 0;
}

unsigned int input_pulsador_bajar(void) {
    static int contador = 0;
    if (pulsador_bajar) {
        if (++contador > 10000) {
            contador = 0;
            return 1;
        }
    } else {
        contador = 0;
        return 0;
    }
    return 0;
}

unsigned int input_pulsador_ok(void) {
    static int contador = 0;
    if (pulsador_ok) {
        if (++contador > 10000) {
            contador = 0;
            return 1;
        }
    } else {
        contador = 0;
        return 0;
    }
    return 0;
}
#endif	/* INPUT_H */

