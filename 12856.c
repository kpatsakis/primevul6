static int selinux_sb_eat_lsm_opts(char *options, void **mnt_opts)
{
	char *from = options;
	char *to = options;
	bool first = true;
	int rc;

	while (1) {
		int len = opt_len(from);
		int token;
		char *arg = NULL;

		token = match_opt_prefix(from, len, &arg);

		if (token != Opt_error) {
			char *p, *q;

			/* strip quotes */
			if (arg) {
				for (p = q = arg; p < from + len; p++) {
					char c = *p;
					if (c != '"')
						*q++ = c;
				}
				arg = kmemdup_nul(arg, q - arg, GFP_KERNEL);
				if (!arg) {
					rc = -ENOMEM;
					goto free_opt;
				}
			}
			rc = selinux_add_opt(token, arg, mnt_opts);
			if (unlikely(rc)) {
				kfree(arg);
				goto free_opt;
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

free_opt:
	if (*mnt_opts) {
		selinux_free_mnt_opts(*mnt_opts);
		*mnt_opts = NULL;
	}
	return rc;
}