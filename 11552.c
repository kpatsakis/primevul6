default_bg(NCURSES_SP_DCL0)
{
    return SP_PARM != 0 ? SP_PARM->_default_bg : COLOR_BLACK;
}