_nc_mouse_wrap(SCREEN *sp)
/* release mouse -- called by endwin() before shellout/exit */
{
    TR(MY_TRACE, ("_nc_mouse_wrap() called"));

    switch (sp->_mouse_type) {
    case M_XTERM:
	if (sp->_mouse_mask)
	    mouse_activate(sp, FALSE);
	break;
#if USE_GPM_SUPPORT
	/* GPM: pass all mouse events to next client */
    case M_GPM:
	if (sp->_mouse_mask)
	    mouse_activate(sp, FALSE);
	break;
#endif
#if USE_SYSMOUSE
    case M_SYSMOUSE:
	mouse_activate(sp, FALSE);
	break;
#endif
#ifdef USE_TERM_DRIVER
    case M_TERM_DRIVER:
	mouse_activate(sp, FALSE);
	break;
#endif
    case M_NONE:
	break;
    }
}