open_input(const char *filename, char *alt_file)
{
    FILE *fp;
    struct stat sb;
    int mode;

    if (!strcmp(filename, "-")) {
	fp = copy_input(stdin, STDIN_NAME, alt_file);
    } else if (stat(filename, &sb) < 0) {
	fprintf(stderr, "%s: %s %s\n", _nc_progname, filename, strerror(errno));
	ExitProgram(EXIT_FAILURE);
    } else if ((mode = (sb.st_mode & S_IFMT)) == S_IFDIR
	       || (mode != S_IFREG && mode != S_IFCHR && mode != S_IFIFO)) {
	fprintf(stderr, "%s: %s is not a file\n", _nc_progname, filename);
	ExitProgram(EXIT_FAILURE);
    } else {
	fp = fopen(filename, "r");

	if (fp == 0) {
	    fprintf(stderr, "%s: Can't open %s\n", _nc_progname, filename);
	    ExitProgram(EXIT_FAILURE);
	}
	if (mode != S_IFREG) {
	    if (alt_file != 0) {
		FILE *fp2 = copy_input(fp, filename, alt_file);
		fp = fp2;
	    } else {
		fprintf(stderr, "%s: %s is not a file\n", _nc_progname, filename);
		ExitProgram(EXIT_FAILURE);
	    }
	}
    }
    return fp;
}