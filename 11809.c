_nc_scrolln(int n, int top, int bot, int maxy)
{
    return NCURSES_SP_NAME(_nc_scrolln) (CURRENT_SCREEN, n, top, bot, maxy);
}