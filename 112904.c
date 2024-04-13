static int32_t parse_len(const char *line)
{
	char num[PKT_LEN_SIZE + 1];
	int i, k, error;
	int32_t len;
	const char *num_end;

	memcpy(num, line, PKT_LEN_SIZE);
	num[PKT_LEN_SIZE] = '\0';

	for (i = 0; i < PKT_LEN_SIZE; ++i) {
		if (!isxdigit(num[i])) {
			/* Make sure there are no special characters before passing to error message */
			for (k = 0; k < PKT_LEN_SIZE; ++k) {
				if(!isprint(num[k])) {
					num[k] = '.';
				}
			}
			
			giterr_set(GITERR_NET, "invalid hex digit in length: '%s'", num);
			return -1;
		}
	}

	if ((error = git__strtol32(&len, num, &num_end, 16)) < 0)
		return error;

	return len;
}