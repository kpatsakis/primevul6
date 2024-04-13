drv_hwlabel(TERMINAL_CONTROL_BLOCK * TCB, int labnum, char *text)
{
    SCREEN *sp = TCB->csp;

    AssertTCB();
    if (labnum > 0 && labnum <= num_labels) {
	NCURSES_PUTP2("plab_norm",
		      TPARM_2(plab_norm, labnum, text));
    }
}