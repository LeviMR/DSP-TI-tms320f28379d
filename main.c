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
    setupADC();
    setupPWM();
    setupLED();

    // 5. Habilita Interrupções no CPU e PIE
    IER |= M_INT1;                     // Habilita Grupo 1 da CPU (onde está o ADCA1)
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;  // Habilita ADCA1
    EINT;                              // Habilita global (INTM)
    ERTM;                              // Habilita real-time (DBGM)

    while(1) {
        // A lógica de controle do seu Cuk pode ser colocada aqui
        // ou dentro da própria ISR para maior velocidade.

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
