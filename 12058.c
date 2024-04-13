qf_set_cwindow_options(void)
{
    // switch off 'swapfile'
    set_option_value_give_err((char_u *)"swf", 0L, NULL, OPT_LOCAL);
    set_option_value_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_value_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
#ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
#endif
#ifdef FEAT_FOLDING
    set_option_value_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
#endif
}