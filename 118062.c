change_inv_order(op)
char *op;
{
    int oc_sym, num;
    char *sp, buf[BUFSZ];

    num = 0;
#ifndef GOLDOBJ
    if (!index(op, GOLD_SYM))
	buf[num++] = COIN_CLASS;
#else
    /*  !!!! probably unnecessary with gold as normal inventory */
#endif

    for (sp = op; *sp; sp++) {
	oc_sym = def_char_to_objclass(*sp);
	/* reject bad or duplicate entries */
	if (oc_sym == MAXOCLASSES ||
		oc_sym == RANDOM_CLASS || oc_sym == ILLOBJ_CLASS ||
		!index(flags.inv_order, oc_sym) || index(sp+1, *sp))
	    return 0;
	/* retain good ones */
	buf[num++] = (char) oc_sym;
    }
    buf[num] = '\0';

    /* fill in any omitted classes, using previous ordering */
    for (sp = flags.inv_order; *sp; sp++)
	if (!index(buf, *sp)) {
	    buf[num++] = *sp;
	    buf[num] = '\0';	/* explicitly terminate for next index() */
	}

    Strcpy(flags.inv_order, buf);
    return 1;
}