static bool valid_arg_len(struct linux_binprm *bprm, long len)
{
	return len <= MAX_ARG_STRLEN;
}