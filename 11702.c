handle_wheel(SCREEN *sp, MEVENT * eventp, int button, int wheel)
{
    bool result = TRUE;

    switch (button & 3) {
    case 0:
	if (wheel) {
	    eventp->bstate = MASK_PRESS(4);
	    /* Do not record in sp->_mouse_bstate; there will be no
	     * corresponding release event.
	     */
	} else {
	    PRESS_POSITION(1);
	}
	break;
    case 1:
	if (wheel) {
#if NCURSES_MOUSE_VERSION >= 2
	    eventp->bstate = MASK_PRESS(5);
	    /* See comment above for button 4 */
#else
	    /* Ignore this event as it is not a true press of the button */
	    eventp->bstate = REPORT_MOUSE_POSITION;
#endif
	} else {
	    PRESS_POSITION(2);
	}
	break;
    case 2:
	PRESS_POSITION(3);
	break;
    default:
	result = FALSE;
	break;
    }
    return result;
}