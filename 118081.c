add_autopickup_exception(mapping)
const char *mapping;
{
	struct autopickup_exception *ape, **apehead;
	char text[256], *text2;
	int textsize = 0;
	boolean grab = FALSE;

	if (sscanf(mapping, "\"%255[^\"]\"", text) == 1) {
		text2 = &text[0];
		if (*text2 == '<') {		/* force autopickup */
			grab = TRUE;
			++text2;
		} else if (*text2 == '>') {	/* default - Do not pickup */
			grab = FALSE;
			++text2;
		}
		textsize = strlen(text2);
		apehead = (grab) ? &iflags.autopickup_exceptions[AP_GRAB] :
				   &iflags.autopickup_exceptions[AP_LEAVE];
		ape = (struct autopickup_exception *)
				alloc(sizeof(struct autopickup_exception));
		ape->pattern = (char *) alloc(textsize+1);
		Strcpy(ape->pattern, text2);
		ape->grab = grab;
		if (!*apehead) ape->next = (struct autopickup_exception *)0;
		else ape->next = *apehead;
		*apehead = ape;
	} else {
	    raw_print("syntax error in AUTOPICKUP_EXCEPTION");
	    return 0;
	}
	return 1;
}