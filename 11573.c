NCURSES_SP_NAME(mouseinterval) (NCURSES_SP_DCLx int maxclick)
/* set the maximum mouse interval within which to recognize a click */
{
    int oldval;

    T((T_CALLED("mouseinterval(%p,%d)"), (void *) SP_PARM, maxclick));

    if (SP_PARM != 0) {
	oldval = SP_PARM->_maxclick;
	if (maxclick >= 0)
	    SP_PARM->_maxclick = maxclick;
    } else {
	oldval = DEFAULT_MAXCLICK;
    }

    returnCode(oldval);
}