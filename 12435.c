nv_clear(cmdarg_T *cap)
{
    if (!checkclearop(cap->oap))
    {
#ifdef FEAT_SYN_HL
	// Clear all syntax states to force resyncing.
	syn_stack_free_all(curwin->w_s);
# ifdef FEAT_RELTIME
	{
	    win_T *wp;

	    FOR_ALL_WINDOWS(wp)
		wp->w_s->b_syn_slow = FALSE;
	}
# endif
#endif
	redraw_later(CLEAR);
#if defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	if (!gui.in_use)
# endif
	    resize_console_buf();
#endif
    }
}