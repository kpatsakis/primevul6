is_wc2_option(optnam)
const char *optnam;
{
	int k = 0;
	while (wc2_options[k].wc_name) {
		if (strcmp(wc2_options[k].wc_name, optnam) == 0)
			return TRUE;
		k++;
	}
	return FALSE;
}