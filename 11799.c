ClrBlank(NCURSES_SP_DCLx WINDOW *win)
{
    NCURSES_CH_T blank = blankchar;
    if (back_color_erase)
	AddAttr(blank, (AttrOf(BCE_BKGD(SP_PARM, win)) & BCE_ATTRS));
    return blank;
}