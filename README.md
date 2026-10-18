# 2026_C_PL_CAMERA
2026 C PL code for BigBrother camera controller based on a xiao esp32c3, hardware details can be found on the ert wiki

## CAN MOSFET revision
All original FSM functions, declarations and state IDs are retained.
Only fsm.cpp changes executable behavior; all other source files are original.

Active path:
INIT (off) -> START_REC -> TRY_START (MOSFET HIGH) -> WAIT_FOR_START
-> RECORDING -> STOP_REC -> TRY_STOP (MOSFET LOW) -> WAIT_FOR_STOP -> ENDED.
START_REC after ENDED starts a new cycle. ABORT powers off; RECOVER retains
the original cooldown behavior. STOP during startup cancels startup.

Startup no longer powers the camera automatically. UART_INIT and the manual
state functions remain defined but are bypassed in normal transitions.
TRY_START/TRY_STOP use powerOn(true)/powerOff directly, not the UART recording
functions. Original UART helpers remain available but are not called by the FSM.
Current-based power verification, retry limits, status LEDs and CAN health
packets remain. Start retries hold power HIGH rather than power-cycle it.
Automatic recording-current recovery is bypassed because it cannot verify
recording in this power-only mode.

Original TRIG selection is unchanged: TRIG must be LOW for CAN operation.
Camera must auto-record on power-up. STOP cuts power; it is not a graceful
recording/file-close command. The original health recording bit is still a
current-based estimate, not confirmed recording status.

Validation: host FSM tests using stubbed platform functions; no ESP32 build
or hardware test.
