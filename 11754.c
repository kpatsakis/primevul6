wmouse_trafo(const WINDOW *win, int *pY, int *pX, bool to_screen)
{
    bool result = FALSE;

    T((T_CALLED("wmouse_trafo(%p,%p,%p,%d)"),
       (const void *) win,
       (void *) pY,
       (void *) pX,
       to_screen));

    if (win && pY && pX) {
	int y = *pY;
	int x = *pX;

	if (to_screen) {
	    y += win->_begy + win->_yoffset;
	    x += win->_begx;
	    if (wenclose(win, y, x))
		result = TRUE;
	} else {
	    if (wenclose(win, y, x)) {
		y -= (win->_begy + win->_yoffset);
		x -= win->_begx;
		result = TRUE;
	    }
	}
	if (result) {
	    *pX = x;
	    *pY = y;
	}
    }
    returnBool(result);
}