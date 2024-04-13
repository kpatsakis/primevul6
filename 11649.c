color_content(NCURSES_COLOR_T color,
	      NCURSES_COLOR_T *r,
	      NCURSES_COLOR_T *g,
	      NCURSES_COLOR_T *b)
{
    return NCURSES_SP_NAME(color_content) (CURRENT_SCREEN, color, r, g, b);
}