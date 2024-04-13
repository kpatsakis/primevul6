count_ape_maps(leave, grab)
int *leave, *grab;
{
	struct autopickup_exception *ape;
	int pass, totalapes, numapes[2] = {0,0};

	for (pass = AP_LEAVE; pass <= AP_GRAB; ++pass) {
		ape = iflags.autopickup_exceptions[pass];
		while(ape) {
			ape = ape->next;
			numapes[pass]++;
		}
	}
	totalapes = numapes[AP_LEAVE] + numapes[AP_GRAB];
	if (leave) *leave = numapes[AP_LEAVE];
	if (grab) *grab = numapes[AP_GRAB];
	return totalapes;
}