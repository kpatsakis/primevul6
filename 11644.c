NCURSES_SP_NAME(color_content) (NCURSES_SP_DCLx
				NCURSES_COLOR_T color,
				NCURSES_COLOR_T *r,
				NCURSES_COLOR_T *g,
				NCURSES_COLOR_T *b)
{
    int my_r, my_g, my_b;
    int rc = _nc_color_content(SP_PARM, color, &my_r, &my_g, &my_b);
    if (rc == OK) {
	*r = limit_COLOR(my_r);
	*g = limit_COLOR(my_g);
	*b = limit_COLOR(my_b);
    }
    return rc;
}