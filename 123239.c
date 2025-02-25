static char * r_cmd_replace_name_def (const char *s_new, ut32 replace_len, const char *s_old, ut32 match_len, const char *buffer, ut32 buf_len, ut32 *res_len) {
	const char * fmt = "L%s;";
	char *s_new_ref = s_new && replace_len > 0 ? malloc (3 + replace_len) : NULL;
	char *s_old_ref = s_old && match_len > 0 ? malloc (3 + match_len) : NULL;
	char *result = NULL;
	*res_len = 0;
	if (s_new_ref && s_old_ref) {
		snprintf (s_new_ref, replace_len+3, fmt, s_new);
		snprintf (s_old_ref, match_len+3, fmt, s_old);
		result = r_cmd_replace_name (s_new_ref, replace_len+2, s_old_ref, match_len+2, buffer, buf_len, res_len);
	}
	free (s_new_ref);
	free (s_old_ref);
	return result;
}