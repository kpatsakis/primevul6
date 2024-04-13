nv_suspend(cmdarg_T *cap)
{
    clearop(cap->oap);
    if (VIsual_active)
	end_visual_mode();		// stop Visual mode
    do_cmdline_cmd((char_u *)"stop");
}