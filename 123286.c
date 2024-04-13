static char * r_cmd_replace_name (const char *s_new, ut32 replace_len, const char *s_old, ut32 match_len, const char *buffer, ut32 buf_len, ut32 *res_len) {
	ut32 num_occurrences = 0, i = 0;
	char * result = NULL, *p_result = NULL;

	num_occurrences = r_cmd_get_num_classname_str_occ (buffer, s_old);
	*res_len = 0;
	if (num_occurrences > 0 && replace_len > 0 && s_old) {
		ut32 consumed = 0;
		const char * next = r_cmd_get_next_classname_str (buffer+consumed, s_old);
		IFDBG r_cons_printf ("Replacing \"%s\" with \"%s\" in: %s\n", s_old, s_new, buffer);
		result = malloc (num_occurrences*replace_len + buf_len);
		memset (result, 0, num_occurrences*replace_len + buf_len);
		p_result = result;
		while (next && consumed < buf_len) {
			// replace up to next
			IFDBG r_cons_printf ("next: \"%s\", len to: %d\n", next, next-buffer );
			for (; buffer + consumed < next  && consumed < buf_len; consumed++, p_result++) {
				*p_result = *(buffer + consumed);
				(*res_len)++;
			}

			for (i=0; i < replace_len; i++,  p_result++){
				*p_result = *(s_new + i);
				(*res_len)++;
			}
			consumed += match_len;
			next = r_cmd_get_next_classname_str (buffer+consumed, s_old);
		}
		IFDBG r_cons_printf ("Found last occurrence of: \"%s\", remaining: %s\n", s_old, buffer+consumed);
		IFDBG r_cons_printf ("result is: \"%s\"\n", result);
		for (; consumed < buf_len; consumed++, p_result++, (*res_len)++) {
			*p_result = *(buffer + consumed);
		}
		IFDBG r_cons_printf ("Old: %s\nNew: %s\n", buffer, result);
	}
	return result;
}