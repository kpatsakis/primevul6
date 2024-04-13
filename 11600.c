NCURSES_SP_NAME(extended_color_content) (NCURSES_SP_DCLx
					 int color,
					 int *r, int *g, int *b)
{
    return _nc_color_content(SP_PARM, color, r, g, b);
}