static int get_pass(rzip_control *control, char *s)
{
	int len;

	memset(s, 0, PASS_LEN - SALT_LEN);
	if (control->passphrase)
		strncpy(s, control->passphrase, PASS_LEN - SALT_LEN - 1);
	else if (unlikely(fgets(s, PASS_LEN - SALT_LEN, stdin) == NULL))
		failure_return(("Failed to retrieve passphrase\n"), -1);
	len = strlen(s);
	if (len > 0 && ('\r' ==  s[len - 1] || '\n' == s[len - 1]))
		s[len - 1] = '\0';
	if (len > 1 && ('\r' ==  s[len - 2] || '\n' == s[len - 2]))
		s[len - 2] = '\0';
	len = strlen(s);
	if (unlikely(0 == len))
		failure_return(("Empty passphrase\n"), -1);
	return len;
}