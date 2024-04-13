remove_autopickup_exception(whichape)
struct autopickup_exception *whichape;
{
    struct autopickup_exception *ape, *prev = 0;
    int chain = whichape->grab ? AP_GRAB : AP_LEAVE;

    for (ape = iflags.autopickup_exceptions[chain]; ape;) {
	if (ape == whichape) {
	    struct autopickup_exception *freeape = ape;
	    ape = ape->next;
	    if (prev) prev->next = ape;
	    else iflags.autopickup_exceptions[chain] = ape;
	    free(freeape->pattern);
	    free(freeape);
	} else {
	    prev = ape;
	    ape = ape->next;
	}
    }
}