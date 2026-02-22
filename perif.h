/*
 * Perif_Setup.h
 *
 *  Created on: 27 de jan de 2026
 *      Author: levif
 */

#ifndef PERIF_H_
#define PERIF_H_
#include "F28x_Project.h"

// --- Cálculos de Tempo (100MHz SYSCLK) ---
// Freq = SYSCLK / (2 * TBPRD) -> 100MHz / (2 * 2000) = 25kHz
#define PWM_PERIOD     2000

// Fator de conversão (Sensor 1: 33k/100R)
#define K_CONV_BRUTA 0.0973388f

// Fator para Saída DC: (3.0V / 4095 bits) * (200V_real / 2.7V_pino)
#define K_SAIDA_DC      0.0542670f

// Fator para corrente de entrada
#define K_CORRENTE_ACS_ENTRADA 10.0f

// Fator para a corrente de saida
#define K_CORRENTE_ACS_SAIDA 10.0f

// Variável global externa para acesso no main.c
extern volatile float vRedeInstantanea;
extern volatile float vSaidaMedida;
extern volatile float iEntradaMedida;
extern volatile float iSaidaMedida;

// Protótipos das funções de configuração
void setupADC(void);
void setupPWM(void);
void setupLED(void);

// Protótipo da ISR (Rotina de Interrupção)
__interrupt void adca1_isr(void);

#endif /* PERIF_H_ */
