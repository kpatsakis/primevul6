mypair(int fg, int bg)
{
    int pair = (fg * COLORS) + bg;
    return (pair >= COLOR_PAIRS) ? -1 : pair;
}