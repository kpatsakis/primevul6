pair_content(NCURSES_COLOR_T pair, NCURSES_COLOR_T *f, NCURSES_COLOR_T *b)
{
    return NCURSES_SP_NAME(pair_content) (CURRENT_SCREEN, pair, f, b);
}