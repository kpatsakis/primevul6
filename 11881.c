_nc_mouse_resume(SCREEN *sp)
/* re-connect to mouse -- called by doupdate() after shellout */
{
    TR(MY_TRACE, ("_nc_mouse_resume() called"));

    switch (sp->_mouse_type) {
    case M_XTERM:
	/* xterm: re-enable reporting */
	if (sp->_mouse_mask)
	    mouse_activate(sp, TRUE);
	break;

#if USE_GPM_SUPPORT
    case M_GPM:
	/* GPM: reclaim our event set */
	if (sp->_mouse_mask)
	    mouse_activate(sp, TRUE);
	break;
#endif

#if USE_SYSMOUSE
    case M_SYSMOUSE:
	mouse_activate(sp, TRUE);
	break;
#endif

#ifdef USE_TERM_DRIVER
    case M_TERM_DRIVER:
	mouse_activate(sp, TRUE);
	break;
#endif

    case M_NONE:
	break;
    }
}