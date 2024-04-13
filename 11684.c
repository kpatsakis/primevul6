extended_color_content(int color, int *r, int *g, int *b)
{
    return NCURSES_SP_NAME(extended_color_content) (CURRENT_SCREEN,
						    color,
						    r, g, b);
}