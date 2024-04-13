_nc_trim_sgr0(TERMTYPE2 *tp)
{
    char *result = exit_attribute_mode;

    T((T_CALLED("_nc_trim_sgr0()")));

    if (PRESENT(exit_attribute_mode)
	&& PRESENT(set_attributes)) {
	bool found = FALSE;
	char *on = set_attribute_9(tp, 1);
	char *off = set_attribute_9(tp, 0);
	char *end = strdup(exit_attribute_mode);
	char *tmp;
	size_t i, j, k;

	TR(TRACE_DATABASE, ("checking if we can trim sgr0 based on sgr"));
	TR(TRACE_DATABASE, ("sgr0       %s", _nc_visbuf(end)));
	TR(TRACE_DATABASE, ("sgr(9:off) %s", _nc_visbuf(off)));
	TR(TRACE_DATABASE, ("sgr(9:on)  %s", _nc_visbuf(on)));

	if (!rewrite_sgr(on, enter_alt_charset_mode)
	    || !rewrite_sgr(off, exit_alt_charset_mode)
	    || !rewrite_sgr(end, exit_alt_charset_mode)) {
	    FreeIfNeeded(off);
	} else if (similar_sgr(off, end)
		   && !similar_sgr(off, on)) {
	    TR(TRACE_DATABASE, ("adjusting sgr(9:off) : %s", _nc_visbuf(off)));
	    result = off;
	    /*
	     * If rmacs is a substring of sgr(0), remove that chunk.
	     */
	    if (PRESENT(exit_alt_charset_mode)) {
		TR(TRACE_DATABASE, ("scan for rmacs %s", _nc_visbuf(exit_alt_charset_mode)));
		j = strlen(off);
		k = strlen(exit_alt_charset_mode);
		if (j > k) {
		    for (i = 0; i <= (j - k); ++i) {
			unsigned k2 = compare_part(exit_alt_charset_mode,
						   off + i);
			if (k2 != 0) {
			    found = TRUE;
			    chop_out(off, (unsigned) i, (unsigned) (i + k2));
			    break;
			}
		    }
		}
	    }
	    /*
	     * SGR 10 would reset to normal font.
	     */
	    if (!found) {
		if ((i = (size_t) is_csi(off)) != 0
		    && off[strlen(off) - 1] == 'm') {
		    TR(TRACE_DATABASE, ("looking for SGR 10 in %s",
					_nc_visbuf(off)));
		    tmp = skip_zero(off + i);
		    if (tmp[0] == '1'
			&& skip_zero(tmp + 1) != tmp + 1) {
			i = (size_t) (tmp - off);
			if (off[i - 1] == ';')
			    i--;
			j = (size_t) (skip_zero(tmp + 1) - off);
			(void) chop_out(off, (unsigned) i, (unsigned) j);
			found = TRUE;
		    }
		}
	    }
	    if (!found
		&& (tmp = strstr(end, off)) != 0
		&& strcmp(end, off) != 0) {
		i = (size_t) (tmp - end);
		j = strlen(off);
		tmp = strdup(end);
		chop_out(tmp, (unsigned) i, (unsigned) j);
		free(off);
		result = tmp;
	    }
	    TR(TRACE_DATABASE, ("...adjusted sgr0 : %s", _nc_visbuf(result)));
	    if (!strcmp(result, exit_attribute_mode)) {
		TR(TRACE_DATABASE, ("...same result, discard"));
		free(result);
		result = exit_attribute_mode;
	    }
	} else {
	    /*
	     * Either the sgr does not reference alternate character set,
	     * or it is incorrect.  That's too hard to decide right now.
	     */
	    free(off);
	}
	FreeIfNeeded(end);
	FreeIfNeeded(on);
    } else {
	/*
	 * Possibly some applications are confused if sgr0 contains rmacs,
	 * but that would be a different bug report -TD
	 */
    }

    returnPtr(result);
}