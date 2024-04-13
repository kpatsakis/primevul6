drv_dobeepflash(TERMINAL_CONTROL_BLOCK * TCB, int beepFlag)
{
    SCREEN *sp;
    int res = ERR;

    AssertTCB();
    SetSP();

    /* FIXME: should make sure that we are not in altchar mode */
    if (beepFlag) {
	if (bell) {
	    res = NCURSES_PUTP2("bell", bell);
	    NCURSES_SP_NAME(_nc_flush) (sp);
	} else if (flash_screen) {
	    res = NCURSES_PUTP2("flash_screen", flash_screen);
	    NCURSES_SP_NAME(_nc_flush) (sp);
	}
    } else {
	if (flash_screen) {
	    res = NCURSES_PUTP2("flash_screen", flash_screen);
	    NCURSES_SP_NAME(_nc_flush) (sp);
	} else if (bell) {
	    res = NCURSES_PUTP2("bell", bell);
	    NCURSES_SP_NAME(_nc_flush) (sp);
	}
    }
    return res;
}