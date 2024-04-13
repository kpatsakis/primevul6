NCURSES_SP_NAME(can_change_color) (NCURSES_SP_DCL)
{
    int result = FALSE;

    T((T_CALLED("can_change_color(%p)"), (void *) SP_PARM));

    if (HasTerminal(SP_PARM) && (CanChange != 0)) {
	result = TRUE;
    }

    returnCode(result);
}