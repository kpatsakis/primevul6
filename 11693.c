NCURSES_SP_NAME(init_color) (NCURSES_SP_DCLx
			     NCURSES_COLOR_T color,
			     NCURSES_COLOR_T r,
			     NCURSES_COLOR_T g,
			     NCURSES_COLOR_T b)
{
    return _nc_init_color(SP_PARM, color, r, g, b);
}