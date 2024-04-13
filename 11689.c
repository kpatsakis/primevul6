set_colors(int fg, int bg)
{
    int pair = mypair(fg, bg);
    if (pair > 0) {
	(void) color_set((short) pair, NewPair(pair));
    }
}