NCURSES_SP_NAME(init_extended_color) (NCURSES_SP_DCLx
				      int color,
				      int r, int g, int b)
{
    return _nc_init_color(SP_PARM, color, r, g, b);
}