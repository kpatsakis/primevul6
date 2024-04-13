make_namelist(char *src)
{
    char **dst = 0;

    char *s, *base;
    unsigned pass, n, nn;
    char buffer[BUFSIZ];

    if (src == 0) {
	/* EMPTY */ ;
    } else if (strchr(src, '/') != 0) {		/* a filename */
	FILE *fp = open_input(src, (char *) 0);

	for (pass = 1; pass <= 2; pass++) {
	    nn = 0;
	    while (fgets(buffer, sizeof(buffer), fp) != 0) {
		if ((s = stripped(buffer)) != 0) {
		    if (dst != 0)
			dst[nn] = s;
		    else
			free(s);
		    nn++;
		}
	    }
	    if (pass == 1) {
		if ((dst = typeCalloc(char *, nn + 1)) == 0)
		      failed("make_namelist");
		rewind(fp);
	    }
	}
	fclose(fp);
    } else {			/* literal list of names */
	for (pass = 1; pass <= 2; pass++) {
	    for (n = nn = 0, base = src;; n++) {
		int mark = src[n];
		if (mark == ',' || mark == '\0') {
		    if (pass == 1) {
			nn++;
		    } else {
			src[n] = '\0';
			if ((s = stripped(base)) != 0)
			    dst[nn++] = s;
			base = &src[n + 1];
		    }
		}
		if (mark == '\0')
		    break;
	    }
	    if (pass == 1) {
		if ((dst = typeCalloc(char *, nn + 1)) == 0)
		      failed("make_namelist");
	    }
	}
    }
    if (showsummary && (dst != 0)) {
	fprintf(log_fp, "Entries that will be compiled:\n");
	for (n = 0; dst[n] != 0; n++)
	    fprintf(log_fp, "%u:%s\n", n + 1, dst[n]);
    }
    return dst;
}