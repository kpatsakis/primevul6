hit_return_msg(void)
{
    int		save_p_more = p_more;

    p_more = FALSE;	// don't want to see this message when scrolling back
    if (msg_didout)	// start on a new line
	msg_putchar('\n');
    if (got_int)
	msg_puts(_("Interrupt: "));

    msg_puts_attr(_("Press ENTER or type command to continue"), HL_ATTR(HLF_R));
    if (!msg_use_printf())
	msg_clr_eos();
    p_more = save_p_more;
}