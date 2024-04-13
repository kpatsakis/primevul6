copy_input(FILE *source, const char *filename, char *alt_file)
{
    char my_altfile[PATH_MAX];
    FILE *result = 0;
    FILE *target = 0;
    int ch;

    if (alt_file == 0)
	alt_file = my_altfile;

    if (source == 0) {
	failed("copy_input (source)");
    } else if ((target = open_tempfile(alt_file)) == 0) {
	failed("copy_input (target)");
    } else {
	clearerr(source);
	for (;;) {
	    ch = fgetc(source);
	    if (feof(source)) {
		break;
	    } else if (ferror(source)) {
		failed(filename);
	    } else if (ch == 0) {
		/* don't loop in case someone wants to convert /dev/zero */
		fprintf(stderr, "%s: %s is not a text-file\n", _nc_progname, filename);
		ExitProgram(EXIT_FAILURE);
	    }
	    fputc(ch, target);
	}
	fclose(source);
	/*
	 * rewind() does not force the target file's data to disk (not does
	 * fflush()...).  So open a second stream on the data and then close
	 * the one that we were writing on before starting to read from the
	 * second stream.
	 */
	result = fopen(alt_file, "r+");
	fclose(target);
	to_remove = strdup(alt_file);
    }
    return result;
}