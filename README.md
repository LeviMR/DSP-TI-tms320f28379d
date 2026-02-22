Phase-Synchronized ePWM Control (C2000)This project implements a high-precision PWM synchronization strategy using the TMS320F2837xD (Delfino) microcontroller. It is designed for power electronics applications, such as Full-Bridge converters or interleaved topologies, where precise phase shifting between multiple PWM modules is critical.

Features
Master-Slave Synchronization: Uses ePWM1 as the master clock source to drive a synchronization chain through ePWM2-ePWM5 up to ePWM6 and ePWM8.Dual-Phase Output:ePWM6B: 0° Phase (Synchronized with Master).ePWM8A: 180° Phase shift.High Frequency: Configured for 25 kHz switching frequency.Adjustable Duty Cycle: Default set to 25% (0.25).Up-Down Count Mode: Implements a symmetrical triangular carrier to reduce harmonic distortion.🛠 Hardware ConfigurationThe code maps the following signals to physical pins:PWM ModuleFunctionGPIOPin (Approx.)PhaseePWM1AMaster RefGPIO0400°ePWM6BSlave 1GPIO11750°ePWM8ASlave 2GPIO1474180°.

Technical Specifications
CPU Frequency: 100 MHz (assumed).ePWM Clock: 100 MHz (EPWMCLKDIV = /2).Counter Mode: TB_COUNT_UPDOWN.Calculated Values:TBPRD: 2000 (for 25 kHz real output).CMPA: 500 (for 25% Duty Cycle).

Repository Structure 
main.c: Contains the setupPWM() function and system initialization.F2837xD_Headers: (Optional) Standard TI header files from controlSUITE/C2000Ware..gitignore: Configured to exclude heavy binaries and IDE-specific temporary files.

Installation & UsageEnvironment: 
Install Code Composer Studio (CCS).SDK: Ensure controlSUITE or C2000Ware is installed.Import: Import the project into your workspace.Build: Compile the code and flash it to your F2837xD LaunchPad or custom board.Observation: Use an oscilloscope or logic analyzer on GPIOs 0, 11, and 14 to verify the 180° phase shift.

Year: 2026
This project is for educational purposes.
