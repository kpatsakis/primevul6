R_API ut16 calculate_access_value(const char *access_flags_str, RBinJavaAccessFlags *access_flags) {
	ut16 result = 0;
	ut16 size = strlen (access_flags_str) + 1;
	char *p_flags, *my_flags = malloc (size);
	RBinJavaAccessFlags *iter = NULL;
	if (size < 5 || !my_flags) {
		free (my_flags);
		return result;
	}
	memcpy (my_flags, access_flags_str, size);
	p_flags = strtok (my_flags, " ");
	while (p_flags && access_flags) {
		int idx = 0;
		do {
			iter = &access_flags[idx];
			if (!iter || !iter->str) {
				continue;
			}
			if (iter->len > 0 && iter->len != 16) {
				if (!strncmp (iter->str, p_flags, iter->len)) {
					result |= iter->value;
				}
			}
			idx++;
		} while (access_flags[idx].str != NULL);
		p_flags = strtok (NULL, " ");
	}
	free (my_flags);
	return result;
}