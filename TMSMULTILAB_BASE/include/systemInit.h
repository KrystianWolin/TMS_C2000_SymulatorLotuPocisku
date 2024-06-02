
#pragma once




void SetupCoreSystem();
void SetupPeripherials();
void SetupInterrupts();
void InitFlash(void);
void SetMainClock();
void StartCPU2();
void ResetCPU2();
void WaitCPU2();
void SetupLEDS();
void SetupUARTB();
void StartTimers();
void VerifyOutputs();
