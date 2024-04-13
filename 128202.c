msg_use_printf(void)
{
    return (!msg_check_screen()
#if defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    || (!gui.in_use && !termcap_active)
# else
	    || !termcap_active
# endif
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}