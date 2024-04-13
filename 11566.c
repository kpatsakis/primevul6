drv_testmouse(TERMINAL_CONTROL_BLOCK * TCB,
	      int delay
	      EVENTLIST_2nd(_nc_eventlist * evl))
{
    int rc = 0;
    SCREEN *sp;

    AssertTCB();
    SetSP();

#if USE_SYSMOUSE
    if ((sp->_mouse_type == M_SYSMOUSE)
	&& (sp->_sysmouse_head < sp->_sysmouse_tail)) {
	rc = TW_MOUSE;
    } else
#endif
    {
	rc = TCBOf(sp)->drv->td_twait(TCBOf(sp),
				      TWAIT_MASK,
				      delay,
				      (int *) 0
				      EVENTLIST_2nd(evl));
#if USE_SYSMOUSE
	if ((sp->_mouse_type == M_SYSMOUSE)
	    && (sp->_sysmouse_head < sp->_sysmouse_tail)
	    && (rc == 0)
	    && (errno == EINTR)) {
	    rc |= TW_MOUSE;
	}
#endif
    }
    return rc;
}