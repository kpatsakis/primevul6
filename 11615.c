mousemask(mmask_t newmask, mmask_t * oldmask)
{
    return NCURSES_SP_NAME(mousemask) (CURRENT_SCREEN, newmask, oldmask);
}