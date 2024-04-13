is_wc_option(optnam)
const char *optnam;
{
	int k = 0;
	while (wc_options[k].wc_name) {
		if (strcmp(wc_options[k].wc_name, optnam) == 0)
			return TRUE;
		k++;
	}
	return FALSE;
}