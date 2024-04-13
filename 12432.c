may_clear_cmdline(void)
{
    if (mode_displayed)
	clear_cmdline = TRUE;   // unshow visual mode later
#ifdef FEAT_CMDL_INFO
    else
	clear_showcmd();
#endif
}