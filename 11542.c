set_colors(int fg, int bg)
{
    int pair = mypair(fg, bg);
    if (pair > 0) {
	attron(COLOR_PAIR(mypair(fg, bg)));
    }
}