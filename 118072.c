free_autopickup_exceptions()
{
	struct autopickup_exception *ape;
	int pass;

	for (pass = AP_LEAVE; pass <= AP_GRAB; ++pass) {
		while((ape = iflags.autopickup_exceptions[pass]) != 0) {
			free(ape->pattern);
			iflags.autopickup_exceptions[pass] = ape->next;
			free(ape);
		}
	}
}