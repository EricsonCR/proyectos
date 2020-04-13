/*
 * File:   main.c
 * Author: Ericson
 *
 * Created on 6 de abril de 2020, 12:19 PM
 */

#include <xc.h>
#include "fuses.h"
#include "max7219.h"
#include "input.h"
#include "output.h"

void oscillator_config(void);
void timer_0_config(void);
void timer_0_enable(void);
void timer_0_disable(void);
void display_init(void);
void update_eeprom(void);
void proceso_subir_bajar_tiempo(void);
void proceso_run(void);
void proceso_detenido(void);

long k = 0;
int minutos_set = 0;
int segundos_set = 0;
int minutos_temp = 0;
int segundos_temp = 0;
int last_minutos_temp = 0;
int last_segundos_temp = 0;
int proceso = 0;
int flag_segundos = 0;
unsigned int veces_uso = 0;
unsigned int encendido = 0;

void main(void) {

    oscillator_config();
    input_config();
    output_config();
    timer_0_config();
    max7219_config();
    max7219_brillo(8);
    update_eeprom();
    display_init();

    while (1) {

        proceso_subir_bajar_tiempo();

        if (proceso == 1) {
            proceso_run();
        } else {
            proceso_detenido();
        }
    }
}

void oscillator_config(void) {
    OSCCONbits.IRCF = 0b1110;
    OSCCONbits.SCS = 0b00;
    OSCCONbits.SPLLEN = 1;
}

void timer_0_config(void) {
    TMR0 = 0;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111;
    OPTION_REGbits.TMR0CS = 0;

    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 0;
    INTCONbits.GIE = 0;
}

void timer_0_enable(void) {
    TMR0 = 0;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;
}

void timer_0_disable(void) {
    TMR0 = 0;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 0;
    INTCONbits.GIE = 0;
}

void display_init(void) {
    int i = 0, j = 0;
    for (i = 10; i > 0; i--) {
        for (j = 1; j < 9; j++) {
            max7219_writeChar(j, i - 1);
        }
        for (k = 0; k < 200000; k++);
    }
    max7219_points(POINTS_ON, NIVEL_UP);
    max7219_points(POINTS_ON, NIVEL_DOWN);
    max7219_writeChar(1, minutos_set / 10);
    max7219_writeChar(5, minutos_set % 10);
    max7219_writeChar(7, segundos_set / 10);
    max7219_writeChar(3, segundos_set % 10);
    max7219_writeChar(8, minutos_temp / 10);
    max7219_writeChar(6, minutos_temp % 10);
    max7219_writeChar(2, segundos_temp / 10);
    max7219_writeChar(4, segundos_temp % 10);
    output_relay(1);
    for (k = 0; k < 100000; k++);
    output_relay(0);
}

void update_eeprom(void) {
    int var1 = 0, var2 = 0;

    var1 = eeprom_read(0);
    minutos_set = var1;
    var2 = eeprom_read(1);
    segundos_set = var2;
    if (minutos_set == 0xFF || segundos_set == 0xFF) {
        minutos_set = 0;
        segundos_set = 0;
    }

    var1 = eeprom_read(2);
    var2 = eeprom_read(3);
    encendido = var1;
    encendido <<= 8;
    encendido |= var2;
    if (encendido == 0xFFFF) {
        encendido = 0;
        encendido++;
        eeprom_write(2, encendido >> 8);
        eeprom_write(3, encendido);
    } else {
        encendido++;
        eeprom_write(2, encendido >> 8);
        eeprom_write(3, encendido);
    }

    var1 = eeprom_read(4);
    var2 = eeprom_read(5);
    veces_uso = var1;
    veces_uso <<= 8;
    veces_uso |= var2;
    if (veces_uso == 0xFFFF) {
        veces_uso = 0;
        eeprom_write(4, veces_uso >> 8);
        eeprom_write(5, veces_uso);
    }

    max7219_writeChar(1, encendido / 1000);
    max7219_writeChar(5, (encendido % 1000) / 100);
    max7219_writeChar(7, ((encendido % 1000) % 100) / 10);
    max7219_writeChar(3, ((encendido % 1000) % 100) % 10);

    max7219_writeChar(8, veces_uso / 1000);
    max7219_writeChar(6, (veces_uso % 1000) / 100);
    max7219_writeChar(2, ((veces_uso % 1000) % 100) / 10);
    max7219_writeChar(4, ((veces_uso % 1000) % 100) % 10);

    for (k = 0; k < 500000; k++);
}

void proceso_subir_bajar_tiempo(void) {
    if (input_pulsador_subir()) {
        if (++minutos_set == 60) {
            minutos_set = 0;
        }
        max7219_writeChar(1, minutos_set / 10);
        max7219_writeChar(5, minutos_set % 10);
    }

    if (input_pulsador_bajar()) {
        if (++segundos_set == 60) {
            segundos_set = 0;
        }
        max7219_writeChar(7, segundos_set / 10);
        max7219_writeChar(3, segundos_set % 10);
    }
}

void proceso_run(void) {
    if (last_minutos_temp != minutos_temp) {
        last_minutos_temp = minutos_temp;
        max7219_writeChar(8, minutos_temp / 10);
        max7219_writeChar(6, minutos_temp % 10);
    }
    if (last_segundos_temp != segundos_temp) {
        if (flag_segundos) {
            flag_segundos = 0;
            max7219_points(POINTS_ON, NIVEL_DOWN);
        } else {
            flag_segundos = 1;
            max7219_points(POINTS_OFF, NIVEL_DOWN);
        }
        last_segundos_temp = segundos_temp;
        max7219_writeChar(2, segundos_temp / 10);
        max7219_writeChar(4, segundos_temp % 10);
    }

    if (minutos_set == minutos_temp && segundos_set == segundos_temp) {
        max7219_points(POINTS_ON, NIVEL_DOWN);
        max7219_writeChar(8, minutos_temp / 10);
        max7219_writeChar(6, minutos_temp % 10);
        max7219_writeChar(2, segundos_temp / 10);
        max7219_writeChar(4, segundos_temp % 10);
        timer_0_disable();
        output_relay(1);
        for (k = 0; k < 4000000; k++);
        output_relay(0);
        proceso = 0;
        minutos_temp = 0;
        segundos_temp = 0;
        max7219_writeChar(8, minutos_temp / 10);
        max7219_writeChar(6, minutos_temp % 10);
        max7219_writeChar(2, segundos_temp / 10);
        max7219_writeChar(4, segundos_temp % 10);
    }
}

void proceso_detenido(void) {
    if (input_pulsador_ok()) {
        eeprom_write(0, minutos_set);
        eeprom_write(1, segundos_set);
        minutos_temp = 0;
        segundos_temp = 0;
        output_relay(1);
        for (k = 0; k < 100000; k++);
        output_relay(0);
        if (segundos_set > 2 || minutos_set > 0) {
            timer_0_enable();
            proceso = 1;
            veces_uso++;
            eeprom_write(4, veces_uso >> 8);
            eeprom_write(5, veces_uso);
            max7219_points(POINTS_ON, NIVEL_DOWN);
            max7219_writeChar(8, minutos_temp / 10);
            max7219_writeChar(6, minutos_temp % 10);
            max7219_writeChar(2, segundos_temp / 10);
            max7219_writeChar(4, segundos_temp % 10);
        }
    }
}

void __interrupt() myISR(void) {
    static int f = 0;
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
        if (++f >= 125) {
            f = 0;
            if (++segundos_temp == 60) {
                minutos_temp++;
                segundos_temp = 0;
            }
        }
    }
}