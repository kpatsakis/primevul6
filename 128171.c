give_warning_with_source(char_u *message, int hl, int with_source)
{
    // Don't do this for ":silent".
    if (msg_silent != 0)
	return;

    // Don't want a hit-enter prompt here.
    ++no_wait_return;

#ifdef FEAT_EVAL
    set_vim_var_string(VV_WARNINGMSG, message, -1);
#endif
    VIM_CLEAR(keep_msg);
    if (hl)
	keep_msg_attr = HL_ATTR(HLF_W);
    else
	keep_msg_attr = 0;

    if (with_source)
    {
	// Do what msg() does, but with a column offset if the warning should
	// be after the mode message.
	msg_start();
	msg_source(HL_ATTR(HLF_W));
	msg_puts(" ");
	msg_puts_attr((char *)message, HL_ATTR(HLF_W) | MSG_HIST);
	msg_clr_eos();
	(void)msg_end();
    }
    else if (msg_attr((char *)message, keep_msg_attr) && msg_scrolled == 0)
	set_keep_msg(message, keep_msg_attr);

    msg_didout = FALSE;	    // overwrite this message
    msg_nowait = TRUE;	    // don't wait for this message
    msg_col = 0;

    --no_wait_return;
}