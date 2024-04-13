static int smack_sb_eat_lsm_opts(char *options, void **mnt_opts)
{
	char *from = options, *to = options;
	bool first = true;

	while (1) {
		char *next = strchr(from, ',');
		int token, len, rc;
		char *arg = NULL;

		if (next)
			len = next - from;
		else
			len = strlen(from);

		token = match_opt_prefix(from, len, &arg);
		if (token != Opt_error) {
			arg = kmemdup_nul(arg, from + len - arg, GFP_KERNEL);
			rc = smack_add_opt(token, arg, mnt_opts);
			if (unlikely(rc)) {
				kfree(arg);
				if (*mnt_opts)
					smack_free_mnt_opts(*mnt_opts);
				*mnt_opts = NULL;
				return rc;
			}
		} else {
			if (!first) {	// copy with preceding comma
				from--;
				len++;
			}
			if (to != from)
				memmove(to, from, len);
			to += len;
			first = false;
		}
		if (!from[len])
			break;
		from += len + 1;
	}
	*to = '\0';
	return 0;
}