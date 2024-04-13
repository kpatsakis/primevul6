show_sb_text(void)
{
    msgchunk_T	*mp;

    // Only show something if there is more than one line, otherwise it looks
    // weird, typing a command without output results in one line.
    mp = msg_sb_start(last_msgchunk);
    if (mp == NULL || mp->sb_prev == NULL)
	vim_beep(BO_MESS);
    else
    {
	do_more_prompt('G');
	wait_return(FALSE);
    }
}