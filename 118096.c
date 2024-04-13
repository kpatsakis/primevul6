wc2_supported(optnam)
const char *optnam;
{
	int k = 0;
	while (wc2_options[k].wc_name) {
		if (!strcmp(wc2_options[k].wc_name, optnam) &&
		    (windowprocs.wincap2 & wc2_options[k].wc_bit))
			return TRUE;
		k++;
	}
	return FALSE;
}