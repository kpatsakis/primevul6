vgr_display_fname(char_u *fname)
{
    char_u	*p;

    msg_start();
    p = msg_strtrunc(fname, TRUE);
    if (p == NULL)
	msg_outtrans(fname);
    else
    {
	msg_outtrans(p);
	vim_free(p);
    }
    msg_clr_eos();
    msg_didout = FALSE;	    // overwrite this message
    msg_nowait = TRUE;	    // don't wait for this message
    msg_col = 0;
    out_flush();
}