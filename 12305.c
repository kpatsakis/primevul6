adjust_cursor(oparg_T *oap)
{
    // The cursor cannot remain on the NUL when:
    // - the column is > 0
    // - not in Visual mode or 'selection' is "o"
    // - 'virtualedit' is not "all" and not "onemore".
    if (curwin->w_cursor.col > 0 && gchar_cursor() == NUL
		&& (!VIsual_active || *p_sel == 'o')
		&& !virtual_active() && (get_ve_flags() & VE_ONEMORE) == 0)
    {
	--curwin->w_cursor.col;
	// prevent cursor from moving on the trail byte
	if (has_mbyte)
	    mb_adjust_cursor();
	oap->inclusive = TRUE;
    }
}