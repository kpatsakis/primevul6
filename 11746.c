NCURSES_SP_NAME(vidattr) (NCURSES_SP_DCLx chtype newmode)
{
    T((T_CALLED("vidattr(%p,%s)"), (void *) SP_PARM, _traceattr(newmode)));
    returnCode(NCURSES_SP_NAME(vidputs) (NCURSES_SP_ARGx
					 newmode,
					 NCURSES_SP_NAME(_nc_putchar)));
}