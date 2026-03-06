/*
 * ============================================================================
 * PROJETO:       Controle para Cuk CA-CC
 * AUTOR:         Levi Marques Rocha
 * DATA:          22 de Fevereiro de 2026
 * HARDWARE:      TMS320F2837xD (C2000 Delfino)
 * AMBIENTE:      Code Composer Studio (CCS) - controlSUITE
 *
 * DESCRIÇÃO:
 * Configuração de PWMs sincronizados para operação a 25kHz.
 * - ePWM1: Mestre de sincronismo (Referência 0°).
 * - ePWM6: Escravo em fase (0°) - Saída Pino 75 (6B).
 * - ePWM8: Escravo defasado (180°) - Saída Pino 74 (8A).
 * - Razão Cíclica (Duty Cycle): 0.25 (25%).
 *
 * REVISÕES:      V1.0 - Sincronismo estabilizado e frequência ajustada.
 * ============================================================================
 */

#include "F28x_Project.h"
#include "perif.h"

void main(void) {
    // 1. Inicializa Sistema, GPIO e Relógios
    InitSysCtrl();  // Clock está em 100MHz
    InitGpio();

    // 2. Inicializa Tabela de Vetores de Interrupção (PIE)
    DINT;               // Desabilita interrupções globais
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    // 3. Registra a função de interrupção na tabela
    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr;
    EDIS;

    // 4. Configura periféricos (chamando as funções de perif.c)
    setupButton();
    setupADC();
    setupPWM();
    setupLED();


    // ========================================================================
    // NOVA ROTINA DE SEGURANÇA (TRAVA O CÓDIGO AQUI)
    // ========================================================================

    // Enquanto o botão NÃO for pressionado (considerando Pull-up interno, pino em '1')
    // O código fica "preso" neste while e o PWM continua em 0V (travado no setupPWM)
    // uint16_t confirmacao = 0;
    DELAY_US(1000);
    while(GpioDataRegs.GPADAT.bit.GPIO8 == 1) {
        // Pisca o LED rápido para indicar que o sistema está aguardando o "START"
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        DELAY_US(100000); // 100ms
    }

//    // TESTE DE DIAGNÓSTICO DO BOTÃO
//    while(GpioDataRegs.GPADAT.bit.GPIO8 == 1) {
//        GpioDataRegs.GPBSET.bit.GPIO34 = 0; // LED Vermelho ACESO se o pino ler 1
//    }
//    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;     // LED Vermelho APAGA se o pino ler 0 (botão funcionou)

    // Quando o botão é apertado, o código sai do while acima e executa estas linhas:
    EALLOW;
    EPwm6Regs.AQCSFRC.bit.CSFB = 0x0; // Libera o pino 75 para o PWM normal
    EPwm8Regs.AQCSFRC.bit.CSFA = 0x0; // Libera o pino 74 para o PWM normal
    EDIS;

    // Deixa o LED aceso fixo por um breve momento para confirmar a partida
    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
    DELAY_US(500000);

    // ========================================================================

    // 5. Habilita Interrupções no CPU e PIE
    IER |= M_INT1;                     // Habilita Grupo 1 da CPU (onde está o ADCA1)
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;  // Habilita ADCA1
    EINT;                              // Habilita global (INTM)
    ERTM;                              // Habilita real-time (DBGM)

    while(1) {
        // A lógica de controle do seu Cuk pode ser colocada aqui
        // ou dentro da própria ISR para maior velocidade.

        // --- 1. TRAVA DE SEGURANÇA (LIMITADOR) ---
        // Impede que o Duty Cycle ultrapasse 80% do PWM_PERIOD (1600 se o período for 2000)
        if(pwmDutyCicle > 1600) {
            pwmDutyCicle = 1600;
        }

        // ATUALIZAÇÃO EM TEMPO REAL
        // Copia o valor da RAM (que você muda no debugger) para o Hardware
        EALLOW;
        EPwm1Regs.CMPA.bit.CMPA = pwmDutyCicle/4;
        EPwm6Regs.CMPA.bit.CMPA = pwmDutyCicle/4;
        EPwm8Regs.CMPA.bit.CMPA = pwmDutyCicle/4;
        EDIS;

        // 1. BLINK LED (Heartbeat)
        // Inverte o estado do pino GPIO34
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;

        // 2. ATRASO PARA O PISCA-PISCA
        // 1.000.000 microssegundos = 1 segundos
        DELAY_US(1000000);

        // Exemplo: Monitoramento simples
        if(vRedeInstantanea > 311.0f) {
            // Ação de proteção aqui
        }
    }
}
