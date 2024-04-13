static int match_opt_prefix(char *s, int l, char **arg)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(smk_mount_opts); i++) {
		size_t len = smk_mount_opts[i].len;
		if (len > l || memcmp(s, smk_mount_opts[i].name, len))
			continue;
		if (len == l || s[len] != '=')
			continue;
		*arg = s + len + 1;
		return smk_mount_opts[i].opt;
	}
	return Opt_error;
}