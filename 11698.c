wenclose(const WINDOW *win, int y, int x)
/* check to see if given window encloses given screen location */
{
    bool result = FALSE;

    T((T_CALLED("wenclose(%p,%d,%d)"), (const void *) win, y, x));

    if (win != 0) {
	y -= win->_yoffset;
	result = ((win->_begy <= y &&
		   win->_begx <= x &&
		   (win->_begx + win->_maxx) >= x &&
		   (win->_begy + win->_maxy) >= y) ? TRUE : FALSE);
    }
    returnBool(result);
}