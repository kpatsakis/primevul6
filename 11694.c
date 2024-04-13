mypair(int fg, int bg)
{
    int result;
#if HAVE_ALLOC_PAIR
    if (x_option) {
	result = alloc_pair(fg, bg);
    } else
#endif
    {
	int pair = (fg * COLORS) + bg;
	result = (pair >= COLOR_PAIRS) ? -1 : pair;
    }
    return result;
}