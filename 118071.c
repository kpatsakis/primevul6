duplicate_opt_detection(opts, iscompound)
const char *opts;
int iscompound;	/* 0 == boolean option, 1 == compound */
{
	int i, *optptr;
	if (!iscompound && iflags.opt_booldup && initial && from_file) {
	    for (i = 0; boolopt[i].name; i++) {
		if (match_optname(opts, boolopt[i].name, 3, FALSE)) {
			optptr = iflags.opt_booldup + i;
			*optptr += 1;
			if (*optptr > 1) return TRUE;
			else return FALSE;
		}
	    }
	} else if (iscompound && iflags.opt_compdup && initial && from_file) {
	    for (i = 0; compopt[i].name; i++) {
		if (match_optname(opts, compopt[i].name,
				strlen(compopt[i].name), TRUE)) {
			optptr = iflags.opt_compdup + i;
			*optptr += 1;
			if (*optptr > 1) return TRUE;
			else return FALSE;
		}
	    }
	}
	return FALSE;
}