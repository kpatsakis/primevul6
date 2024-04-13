init_color_table(NCURSES_SP_DCL0)
{
    const color_t *tp = DefaultPalette;
    int n;

    assert(tp != 0);

    for (n = 0; n < COLORS; n++) {
	if (InPalette(n)) {
	    SP_PARM->_color_table[n] = tp[n];
	} else {
	    SP_PARM->_color_table[n] = tp[n % MAX_PALETTE];
	    if (UseHlsPalette) {
		SP_PARM->_color_table[n].green = 100;
	    } else {
		if (SP_PARM->_color_table[n].red)
		    SP_PARM->_color_table[n].red = 1000;
		if (SP_PARM->_color_table[n].green)
		    SP_PARM->_color_table[n].green = 1000;
		if (SP_PARM->_color_table[n].blue)
		    SP_PARM->_color_table[n].blue = 1000;
	    }
	}
    }
}