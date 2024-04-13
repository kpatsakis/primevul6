NCURSES_SP_NAME(mousemask) (NCURSES_SP_DCLx mmask_t newmask, mmask_t * oldmask)
/* set the mouse event mask */
{
    mmask_t result = 0;

    T((T_CALLED("mousemask(%p,%#lx,%p)"),
       (void *) SP_PARM,
       (unsigned long) newmask,
       (void *) oldmask));

    if (SP_PARM != 0) {
	if (oldmask)
	    *oldmask = SP_PARM->_mouse_mask;

	if (newmask || SP_PARM->_mouse_initialized) {
	    _nc_mouse_init(SP_PARM);

	    if (SP_PARM->_mouse_type != M_NONE) {
		int b;

		result = newmask &
		    (REPORT_MOUSE_POSITION
		     | BUTTON_ALT
		     | BUTTON_CTRL
		     | BUTTON_SHIFT
		     | BUTTON_PRESSED
		     | BUTTON_RELEASED
		     | BUTTON_CLICKED
		     | BUTTON_DOUBLE_CLICKED
		     | BUTTON_TRIPLE_CLICKED);

		mouse_activate(SP_PARM, (bool) (result != 0));

		SP_PARM->_mouse_mask = result;
		SP_PARM->_mouse_mask2 = result;

		/*
		 * Make a mask corresponding to the states we will need to
		 * retain (temporarily) while building up the state that the
		 * user asked for.
		 */
		for (b = 1; b <= MAX_BUTTONS; ++b) {
		    if (SP_PARM->_mouse_mask2 & MASK_TRIPLE_CLICK(b))
			SP_PARM->_mouse_mask2 |= MASK_DOUBLE_CLICK(b);
		    if (SP_PARM->_mouse_mask2 & MASK_DOUBLE_CLICK(b))
			SP_PARM->_mouse_mask2 |= MASK_CLICK(b);
		    if (SP_PARM->_mouse_mask2 & MASK_CLICK(b))
			SP_PARM->_mouse_mask2 |= (MASK_PRESS(b) |
						  MASK_RELEASE(b));
		}
	    }
	}
    }
    returnMMask(result);
}