_nc_mouse_init(SCREEN *sp)
/* initialize the mouse */
{
    bool result = FALSE;

    if (sp != 0) {
	if (!sp->_mouse_initialized) {
	    int i;

	    sp->_mouse_initialized = TRUE;

	    TR(MY_TRACE, ("_nc_mouse_init() called"));

	    sp->_mouse_eventp = FirstEV(sp);
	    for (i = 0; i < EV_MAX; i++)
		Invalidate(sp->_mouse_events + i);

	    initialize_mousetype(sp);

	    T(("_nc_mouse_init() set mousetype to %d", sp->_mouse_type));
	}
	result = sp->_mouse_initialized;
    }
    return result;
}