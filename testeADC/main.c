#include "F28x_Project.h"
#include "perif.h"

void main(void) {
    // 1. Inicializa Sistema, GPIO e Relógios
    InitSysCtrl();
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
