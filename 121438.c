ShiftOverride(XtermWidget xw, unsigned state, int button)
{
    unsigned check = (state & OurModifiers(xw));
    Boolean result = False;

    if (check & ShiftMask) {
	if (xw->keyboard.shift_escape == ssFalse ||
	    xw->keyboard.shift_escape == ssNever) {
	    result = True;
	} else if (xw->keyboard.shift_escape == ssTrue) {
	    /*
	     * Check if the button is one that we found does not directly use
	     * the shift-modifier in its bindings to select/copy actions.
	     */
	    if (button > 0 && button <= MaxMouseBtn) {
		if (xw->keyboard.shift_buttons & (1U << (button - 1))) {
		    result = True;
		}
	    } else {
		result = True;	/* unlikely, and we don't care */
	    }
	}
    }
    TRACE2(("ShiftOverride ( %#x -> %#x ) %d\n", state, check, result));
    return result;
}