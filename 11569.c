drv_twait(TERMINAL_CONTROL_BLOCK * TCB,
	  int mode,
	  int milliseconds,
	  int *timeleft
	  EVENTLIST_2nd(_nc_eventlist * evl))
{
    SCREEN *sp;

    AssertTCB();
    SetSP();

    return _nc_timed_wait(sp, mode, milliseconds, timeleft EVENTLIST_2nd(evl));
}