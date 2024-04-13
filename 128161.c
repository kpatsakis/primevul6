msg_clr_eos(void)
{
    if (msg_silent == 0)
	msg_clr_eos_force();
}