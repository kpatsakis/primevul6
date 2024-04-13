static int get_mem_option(char *token) {
	// values 4, 8, 12, are unused. XXX to adjust
	const char *options[] = {"sy", "st", "ld", "xxx", "ish", "ishst",
	                         "ishld", "xxx", "nsh", "nshst", "nshld",
	                         "xxx", "osh", "oshst", "oshld", NULL};
	int i = 0;
	while (options[i]) {
		if (!r_str_casecmp (token, options[i])) {
			return 15 - i;
		}
		i++;
	}
	return -1;
}