display_confirm_msg(void)
{
    // avoid that 'q' at the more prompt truncates the message here
    ++confirm_msg_used;
    if (confirm_msg != NULL)
	msg_puts_attr((char *)confirm_msg, HL_ATTR(HLF_M));
    --confirm_msg_used;
}