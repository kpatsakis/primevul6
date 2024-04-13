tparm_setup(const char *string, TPARM_DATA * result)
{
    int rc = OK;

    TPS(out_used) = 0;
    memset(result, 0, sizeof(*result));

    if (string == NULL) {
	TR(TRACE_CALLS, ("%s: format is null", TPS(tname)));
	rc = ERR;
    } else {
#if HAVE_TSEARCH
	TPARM_DATA *fs;
	void *ft;

	result->format = string;
	if ((ft = tfind(result, &cached_tparm, cmp_format)) != 0) {
	    fs = *(TPARM_DATA **) ft;
	    *result = *fs;
	} else
#endif
	{
	    /*
	     * Find the highest parameter-number referred to in the format
	     * string.  Use this value to limit the number of arguments copied
	     * from the variable-length argument list.
	     */
	    result->num_parsed = _nc_tparm_analyze(string,
						   result->p_is_s,
						   &(result->num_popped));
	    if (TPS(fmt_buff) == 0) {
		TR(TRACE_CALLS, ("%s: error in analysis", TPS(tname)));
		rc = ERR;
	    } else {
		int n;

		if (result->num_parsed > NUM_PARM)
		    result->num_parsed = NUM_PARM;
		if (result->num_popped > NUM_PARM)
		    result->num_popped = NUM_PARM;
		result->num_actual = max(result->num_popped, result->num_parsed);

		for (n = 0; n < result->num_actual; ++n) {
		    if (result->p_is_s[n])
			result->tparm_type |= (1 << n);
		}
#if HAVE_TSEARCH
		if ((fs = typeCalloc(TPARM_DATA, 1)) != 0) {
		    *fs = *result;
		    if ((fs->format = strdup(string)) != 0) {
			if ((ft = tsearch(fs, &cached_tparm, cmp_format)) != 0) {
			    ++count_tparm;
			} else {
			    rc = ERR;
			}
		    } else {
			rc = ERR;
		    }
		} else {
		    rc = ERR;
		}
#endif
	    }
	}
    }

    return rc;
}