restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    if (NULL != dirname_now)
    {
	mch_dirname(dirname_now, MAXPATHL);
	if (STRCMP(dirname_start, dirname_now) != 0)
	{
	    // If the directory has changed, change it back by building up an
	    // appropriate ex command and executing it.
	    exarg_T ea;

	    CLEAR_FIELD(ea);
	    ea.arg = dirname_start;
	    ea.cmdidx = (curwin->w_localdir == NULL) ? CMD_cd : CMD_lcd;
	    ex_cd(&ea);
	}
	vim_free(dirname_now);
    }
}