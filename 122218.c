R_API char *retrieve_access_string(ut16 flags, RBinJavaAccessFlags *access_flags) {
	char *outbuffer = NULL, *cur_pos = NULL;
	ut16 i;
	ut16 max_str_len = 0;
	for (i = 0; access_flags[i].str != NULL; i++) {
		if (flags & access_flags[i].value) {
			max_str_len += (strlen (access_flags[i].str) + 1);
			if (max_str_len < strlen (access_flags[i].str)) {
				return NULL;
			}
		}
	}
	max_str_len++;
	outbuffer = (char *) malloc (max_str_len);
	if (outbuffer) {
		memset (outbuffer, 0, max_str_len);
		cur_pos = outbuffer;
		for (i = 0; access_flags[i].str != NULL; i++) {
			if (flags & access_flags[i].value) {
				ut8 len = strlen (access_flags[i].str);
				const char *the_string = access_flags[i].str;
				memcpy (cur_pos, the_string, len);
				memcpy (cur_pos + len, " ", 1);
				cur_pos += len + 1;
			}
		}
		if (cur_pos != outbuffer) {
			*(cur_pos - 1) = 0;
		}
	}
	return outbuffer;
}