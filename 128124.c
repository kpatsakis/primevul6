msg_end(void)
{
    /*
     * If the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    if (!exiting && need_wait_return && !(State & MODE_CMDLINE))
    {
	wait_return(FALSE);
	return FALSE;
    }
    out_flush();
    return TRUE;
}