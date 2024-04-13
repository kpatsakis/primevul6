init_extended_color(int color, int r, int g, int b)
{
    return NCURSES_SP_NAME(init_extended_color) (CURRENT_SCREEN,
						 color,
						 r, g, b);
}