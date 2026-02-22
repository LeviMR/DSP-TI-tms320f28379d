/*
 * Perif_Setup.c
 *
 *  Created on: 27 de jan de 2026
 *      Author: levif
 */


#include <perif.h>

volatile float vRedeInstantanea = 0.0f; // Variável para a tensão de entrada
volatile float vSaidaMedida = 0.0f;     // Nova variável para o sensor de tensão de saída
volatile float iEntradaMedida = 0.0f;   // Variável para a corrente de entrada
volatile float iSaidaMedida = 0.0f;     // Variável para a corrente de saída

void setupLED(void) {
    EALLOW;
    // LED Vermelho da LaunchPad F28379D (GPIO34)
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // Configura como GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;  // Configura como Saída

    // Opcional: LED Azul (GPIO31)
//    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
//    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    EDIS;
}

void setupADC(void) {
    EALLOW;
    // Ajuste para SYSCLK de 200MHz: 200MHz / 4 = 50MHz (Limite do ADC)
    AdcaRegs.ADCCTL2.bit.PRESCALE = 4;
    AdcbRegs.ADCCTL2.bit.PRESCALE = 4;
    AdccRegs.ADCCTL2.bit.PRESCALE = 4;

    // Configura Resolução (12 bits) e modo (Single-ended)
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    // Liga o ADC
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    DELAY_US(1000);

    // Configura SOC0 (Start of Conversion) no pino A0 - J3 30
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;    // Canal A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 14;   // Janela de amostragem
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5;  // Disparo pelo ePWM1 ADCSOCA

    // --- CONFIGURAÇÃO SOC0 NO ADCB (Pino B2 - J3 28) ---
    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;    // Seleciona o canal B2
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = 14;   // Mesma janela de amostragem do A
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 5;  // MESMO disparo (ePWM1) garante simultaneidade

    // --- CONFIGURAÇÃO SOC0 NO ADCC (Pino C3 - J3 24) ---
    AdccRegs.ADCSOC0CTL.bit.CHSEL = 3;    // Seleciona o canal C3
    AdccRegs.ADCSOC0CTL.bit.ACQPS = 14;
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 5;  // Mesmo disparo ePWM1 (Leitura tripla simultânea!)

    // --- CONFIGURAÇÃO DO NOVO CANAL ADCINC2 J3 27 ---
    // Usaremos o SOC1 do módulo C, já que o SOC0 está sendo usado pelo C3
    AdccRegs.ADCSOC1CTL.bit.CHSEL = 2;     // Seleciona o canal C2
    AdccRegs.ADCSOC1CTL.bit.ACQPS = 14;    // Janela de amostragem (igual aos outros)
    AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 5;   // Disparo pelo ePWM1 (Sincronizado)

    // Configura Interrupção do ADCA
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; // Fim da conversão 0 gera INT1
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   // Habilita a interrupção
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Limpa flag inicial
    EDIS;
}

//void setupPWM(void) {
//    EALLOW;
//    // 1. Configuração dos Pinos (Mux)
//    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1; // ePWM6B 75
//    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1; // ePWM8A 74
//
//    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1; // Habilita clock do módulo PWM1
//    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1; // Habilita clock do módulo PWM1
//    CpuSysRegs.PCLKCR2.bit.EPWM8 = 1;
//
//    // Sincronização Global Off para configuração
//    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
//
////    EPwm1Regs.TBPRD = 2500;           // Define frequência (ajustar conforme necessidade)
////    EPwm1Regs.TBCTL.bit.CTRMODE = 0;  // Contagem crescente (Up-count)
////
////    // Configura o trigger do ADC no PWM
////    EPwm1Regs.ETSEL.bit.SOCAEN = 1;   // Habilita SOCA
////    EPwm1Regs.ETSEL.bit.SOCASEL = 1;  // Disparo quando o contador = 0
////    EPwm1Regs.ETPS.bit.SOCAPRD = 1;   // Dispara em todo evento
//
//    // --- EPWM1: Apenas Trigger do ADC (25kHz) ---
//    EPwm1Regs.TBPRD = PWM_PERIOD;
//    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
//    EPwm1Regs.ETSEL.bit.SOCAEN = 1;
//    EPwm1Regs.ETSEL.bit.SOCASEL = TB_CTR_ZERO; // Dispara no zero (centro do PWM)
//    EPwm1Regs.ETPS.bit.SOCAPRD = 1;
//
//    // --- EPWM6: Responsável pela saída 6B --- 75
//    EPwm6Regs.TBPRD = PWM_PERIOD;
//    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
//    EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0;
//    EPwm6Regs.CMPA.bit.CMPA = PWM_PERIOD / 2; // Duty 50%
//
//    // Elas definem o comportamento do sinal A interno, que o Dead-Band usará para criar o B
//    EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;   // Seta (High) quando o contador sobe e atinge CMPA
//    EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR; // Limpa (Low) quando o contador desce e atinge CMPA
//    // =================================
//
//    // Dead Band: Inverte 6B em relação ao sinal A interno
//    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
////    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;      // 6B é o complementar
//    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
//    EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL;
//    EPwm6Regs.DBRED.all = PWM_DEADBAND;
//    EPwm6Regs.DBFED.all = PWM_DEADBAND;
//
//    // --- EPWM8: Responsável pela saída 8A (Complementar ao 6B) ---
//    EPwm8Regs.TBPRD = PWM_PERIOD;
//    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
//    EPwm8Regs.TBCTL.bit.HSPCLKDIV = 0;
//    EPwm8Regs.CMPA.bit.CMPA = PWM_PERIOD / 2;
//
//    // Sincronismo: EPWM8 segue o EPWM6
//    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
//    EPwm8Regs.TBCTL.bit.PHSEN = TB_ENABLE;
//    EPwm8Regs.TBPHS.bit.TBPHS = 0;
//
//    // Lógica 8A: Mesma do 6A interno (Assim ele será oposto ao 6B)
////    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
////    EPwm8Regs.AQCTLA.bit.CAD = AQ_SET;
//
//    EPwm8Regs.AQCTLA.bit.CAU = AQ_SET;
//    EPwm8Regs.AQCTLA.bit.CAD = AQ_CLEAR;
//
//    // Dead Band no 8A para simetria de atraso
//    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
//    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;
//    EPwm8Regs.DBCTL.bit.IN_MODE = DBA_ALL;
//    EPwm8Regs.DBRED.all = PWM_DEADBAND;
//    EPwm8Regs.DBFED.all = PWM_DEADBAND;
//
//    // Ativa sincronismo
//    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
//    EDIS;
//}

//void setupPWM(void) {
//    EALLOW;
//    // --- Configuração de Pinos ---
//    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1; // ePWM6B (Pino 75)
//    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1; // ePWM8A (Pino 74)
//
//    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1;
//    CpuSysRegs.PCLKCR2.bit.EPWM8 = 1;
//    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
//
//    // --- EPWM6 (MESTRE) ---
//    EPwm6Regs.TBPRD = PWM_PERIOD;
//    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
//    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;     // Mestre não recebe fase
//    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Envia sincronismo quando chegar em ZERO
//    EPwm6Regs.CMPA.bit.CMPA = PWM_PERIOD / 2;   // 50% Duty
//
//    // AQ: Configura o sinal A interno
//    EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;
//    EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;
//
//    // Dead-Band: Gera o sinal B complementar ao A (Pino 75)
//    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
//    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;   // Inverte o A para o pino B
//    EPwm6Regs.DBRED.all = PWM_DEADBAND;         // 1us
//    EPwm6Regs.DBFED.all = PWM_DEADBAND;         // 1us
//
//    // --- EPWM8 (ESCRAVO - 180°) ---
//    EPwm8Regs.TBPRD = PWM_PERIOD;
//    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
//
//    // Sincronismo e Fase
//    EPwm8Regs.TBCTL.bit.PHSEN = TB_ENABLE;      // Habilita recebimento de fase
//    EPwm8Regs.TBPHS.bit.TBPHS = PWM_PERIOD/3;     // Carrega o topo (180 graus)
//    EPwm8Regs.TBCTL.bit.PHSDIR = TB_DOWN;       // Ao sincronizar, começa contando para BAIXO
//
//    EPwm8Regs.CMPA.bit.CMPA = PWM_PERIOD / 2;
//
//    // AQ: Mesma lógica do mestre
//    EPwm8Regs.AQCTLA.bit.CAU = AQ_SET;
//    EPwm8Regs.AQCTLA.bit.CAD = AQ_CLEAR;
//
//    // Dead-Band: Gera o sinal A (Pino 74)
//    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
//    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;    // Sinal A direto (Ativo Alto)
//    EPwm8Regs.DBRED.all = PWM_DEADBAND;
//    EPwm8Regs.DBFED.all = PWM_DEADBAND;
//
//    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
//    EDIS;
//}

void setupPWM(void) {

    EALLOW;
    //PWM 1A pino 40
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;     //EPWM1A
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;      //desabilita o pull-up interno

    //PWM6A pino 76
    GpioCtrlRegs.GPAGMUX1.bit.GPIO10 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;

    // Configura GPIO11 como EPWM6B (Pino 75 dependendo do encapsulamento)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO11 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;

    // Configura GPIO14 como EPWM8A (Pino 74 dependendo do encapsulamento)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 =1;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    CpuSysRegs.PCLKCR2.all |= 0x00FF; //Liga clocks de 1 a 8 para o Sync passar
    EDIS;

    // pino 40A 39B
    EPwm1Regs.TBPRD = PWM_PERIOD;           // 25 kHz
    EPwm1Regs.CMPA.bit.CMPA = PWM_PERIOD/4;                  // D = 0,25
    EPwm1Regs.TBCTR = 0x0000;                       // Clear counter

    EPwm1Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;     // Out pulse sincronization, master
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;

    EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;

    // pino 75
    EPwm6Regs.TBPRD = EPwm1Regs.TBPRD;
    EPwm6Regs.CMPA.bit.CMPA = PWM_PERIOD/4;
    EPwm6Regs.TBCTR = 0x0000;

    EPwm6Regs.TBPHS.bit.TBPHS = 0; // Phase is 60o (180/3)
    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // Slave phase
    EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;      // Enable phase loading
    EPwm6Regs.TBCTL.bit.PHSDIR = 1;             // Phase dir
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm6Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm6Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm6Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm6Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;
    EPwm6Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;
    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CAD = AQ_SET;

    // --- PONTES DE SINCRONISMO (ePWM2 até ePWM5 e ePWM7) ---
    // Essencial para o sinal do 1 chegar no 6 e no 8
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;

    //---------------------------------------
    // pino 74
    EPwm8Regs.TBPRD = EPwm1Regs.TBPRD;
    EPwm8Regs.CMPA.bit.CMPA = PWM_PERIOD/4;
    EPwm8Regs.TBCTR = 0x0000;

    EPwm8Regs.TBPHS.bit.TBPHS = EPwm1Regs.TBPRD; // Phase is 180o
    EPwm8Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // Slave phase
    EPwm8Regs.TBCTL.bit.PHSEN = TB_ENABLE;      // Enable phase loading
    EPwm8Regs.TBCTL.bit.PHSDIR = 0;             // Phase dir
    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm8Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm8Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm8Regs.AQCTLA.bit.CAD = AQ_SET;
   //---------------------------------------

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

// Função de Interrupção
__interrupt void adca1_isr(void) {
    // Leitura e conversão para tensão bruta da rede
    vRedeInstantanea = (float)AdcaResultRegs.ADCRESULT0 * K_CONV_BRUTA;

    // 2. Leitura do canal B2 (Saída - ADICIONAR AQUI)
    // Como os triggers são iguais, o resultado de B já estará pronto aqui
    vSaidaMedida = (float)AdcbResultRegs.ADCRESULT0 * K_SAIDA_DC;

    // 3. Corrente de Entrada (ADCC - Pino C3)
    // Sem offset, pois o subtrator já limpou os 2.5V
    iEntradaMedida = (float)AdccResultRegs.ADCRESULT0 * (3.0f / 4095.0f) * K_CORRENTE_ACS_ENTRADA;

    // Leitura do novo sinal no ADCINC2
    // Convertendo para Volts (considerando VREF = 3.0V)
    iSaidaMedida = (float)AdccResultRegs.ADCRESULT1 * (3.0f / 4095.0f)*K_CORRENTE_ACS_SAIDA;

    // Limpa as flags de interrupção
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}




