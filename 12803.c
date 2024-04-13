static int smack_add_opt(int token, const char *s, void **mnt_opts)
{
	struct smack_mnt_opts *opts = *mnt_opts;

	if (!opts) {
		opts = kzalloc(sizeof(struct smack_mnt_opts), GFP_KERNEL);
		if (!opts)
			return -ENOMEM;
		*mnt_opts = opts;
	}
	if (!s)
		return -ENOMEM;

	switch (token) {
	case Opt_fsdefault:
		if (opts->fsdefault)
			goto out_opt_err;
		opts->fsdefault = s;
		break;
	case Opt_fsfloor:
		if (opts->fsfloor)
			goto out_opt_err;
		opts->fsfloor = s;
		break;
	case Opt_fshat:
		if (opts->fshat)
			goto out_opt_err;
		opts->fshat = s;
		break;
	case Opt_fsroot:
		if (opts->fsroot)
			goto out_opt_err;
		opts->fsroot = s;
		break;
	case Opt_fstransmute:
		if (opts->fstransmute)
			goto out_opt_err;
		opts->fstransmute = s;
		break;
	}
	return 0;

out_opt_err:
	pr_warn("Smack: duplicate mount options\n");
	return -EINVAL;
}