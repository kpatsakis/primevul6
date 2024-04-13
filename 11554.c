show_databases(const char *outdir)
{
    bool specific = (outdir != 0) || getenv("TERMINFO") != 0;
    char *result;
    const char *tried = 0;

    if (outdir == 0) {
	outdir = _nc_tic_dir(0);
    }
    if ((result = valid_db_path(outdir)) != 0) {
	printf("%s\n", result);
	free(result);
    } else {
	tried = outdir;
    }

    if ((outdir = _nc_home_terminfo())) {
	if ((result = valid_db_path(outdir)) != 0) {
	    printf("%s\n", result);
	    free(result);
	} else if (!specific) {
	    tried = outdir;
	}
    }

    /*
     * If we can write in neither location, give an error message.
     */
    if (tried) {
	fflush(stdout);
	fprintf(stderr, "%s: %s (no permission)\n", _nc_progname, tried);
	ExitProgram(EXIT_FAILURE);
    }
}