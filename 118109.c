wc_supported(optnam)
const char *optnam;
{
	int k = 0;
	while (wc_options[k].wc_name) {
		if (!strcmp(wc_options[k].wc_name, optnam) &&
		    (windowprocs.wincap & wc_options[k].wc_bit))
			return TRUE;
		k++;
	}
	return FALSE;
}