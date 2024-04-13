static int smack_fs_context_parse_param(struct fs_context *fc,
					struct fs_parameter *param)
{
	struct fs_parse_result result;
	int opt, rc;

	opt = fs_parse(fc, smack_fs_parameters, param, &result);
	if (opt < 0)
		return opt;

	rc = smack_add_opt(opt, param->string, &fc->security);
	if (!rc)
		param->string = NULL;
	return rc;
}