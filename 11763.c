NCURSES_SP_NAME(has_colors) (NCURSES_SP_DCL0)
{
    int code = FALSE;

    (void) SP_PARM;
    T((T_CALLED("has_colors(%p)"), (void *) SP_PARM));
    if (HasTerminal(SP_PARM)) {
#ifdef USE_TERM_DRIVER
	code = HasColor;
#else
	code = ((VALID_NUMERIC(max_colors) && VALID_NUMERIC(max_pairs)
		 && (((set_foreground != NULL)
		      && (set_background != NULL))
		     || ((set_a_foreground != NULL)
			 && (set_a_background != NULL))
		     || set_color_pair)) ? TRUE : FALSE);
#endif
    }
    returnCode(code);
}