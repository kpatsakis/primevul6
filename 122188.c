R_API int r_bin_java_extract_reference_name(const char *input_str, char **ref_str, ut8 array_cnt) {
	char *new_str = NULL;
	ut32 str_len = array_cnt ? (array_cnt + 1) * 2 : 0;
	const char *str_pos = input_str;
	int consumed = 0, len = 0;
	if (!str_pos || *str_pos != 'L' || !*str_pos) {
		return -1;
	}
	consumed++;
	str_pos++;
	while (*str_pos && *str_pos != ';') {
		str_pos++;
		len++;
		consumed++;
	}
	str_pos = input_str + 1;
	free (*ref_str);
	str_len += len;
	*ref_str = malloc (str_len + 1);
	new_str = *ref_str;
	memcpy (new_str, input_str + 1, str_len);
	new_str[str_len] = 0;
	while (*new_str) {
		if (*new_str == '/') {
			*new_str = '.';
		}
		new_str++;
	}
	return len + 2;
}