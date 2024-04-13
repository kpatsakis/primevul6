msg_clr_cmdline(void)
{
    msg_row = cmdline_row;
    msg_col = 0;
    msg_clr_eos_force();
}