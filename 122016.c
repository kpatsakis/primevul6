R_API char *get_type_value_str(const char *arg_str, ut8 array_cnt) {
	ut32 str_len = array_cnt ? (array_cnt + 1) * 2 + strlen (arg_str) : strlen (arg_str);
	char *str = malloc (str_len + 1);
	ut32 bytes_written = snprintf (str, str_len + 1, "%s", arg_str);
	while (array_cnt > 0) {
		bytes_written = snprintf (str + bytes_written, str_len - bytes_written, "[]");
		array_cnt--;
	}
	return str;
}