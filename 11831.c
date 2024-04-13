NCURSES_SP_NAME(reset_color_pairs) (NCURSES_SP_DCL0)
{
    if (SP_PARM != 0) {
	if (SP_PARM->_color_pairs) {
	    _nc_free_ordered_pairs(SP_PARM);
	    free(SP_PARM->_color_pairs);
	    SP_PARM->_color_pairs = 0;
	    SP_PARM->_pair_alloc = 0;
	    ReservePairs(SP_PARM, 16);
	    clearok(CurScreen(SP_PARM), TRUE);
	    touchwin(StdScreen(SP_PARM));
	}
    }
}