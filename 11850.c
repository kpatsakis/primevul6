vid_attr(attr_t newmode, NCURSES_PAIRS_T pair_arg, void *opts)
{
    return NCURSES_SP_NAME(vid_attr) (CURRENT_SCREEN, newmode, pair_arg, opts);
}