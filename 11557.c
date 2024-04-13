drv_hwlabelOnOff(TERMINAL_CONTROL_BLOCK * TCB, int OnFlag)
{
    SCREEN *sp = TCB->csp;

    AssertTCB();
    if (OnFlag) {
	NCURSES_PUTP2("label_on", label_on);
    } else {
	NCURSES_PUTP2("label_off", label_off);
    }
}