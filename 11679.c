main(int argc, char *argv[])
{
    int c, tc = FALSE;

    while ((c = getopt(argc, argv, "c")) != EOF)
	switch (c) {
	case 'c':
	    tc = TRUE;
	    break;
	}

    curr_line = 0;
    for (;;) {
	char buf[BUFSIZ];

	++curr_line;
	if (fgets(buf, sizeof(buf), stdin) == 0)
	    break;
	buf[strlen(buf) - 1] = '\0';
	_nc_set_source(buf);

	if (tc) {
	    char *cp = _nc_infotocap("to termcap", buf, 1);

	    if (cp)
		(void) fputs(cp, stdout);
	} else
	    (void) fputs(_nc_captoinfo("to terminfo", buf, 1), stdout);
	(void) putchar('\n');
    }
    return (0);
}