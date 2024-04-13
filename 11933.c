drv_wrap(SCREEN *sp)
{
    if (sp) {
	sp->_mouse_wrap(sp);
	NCURSES_SP_NAME(_nc_screen_wrap) (sp);
	NCURSES_SP_NAME(_nc_mvcur_wrap) (sp);	/* wrap up cursor addressing */
    }
}