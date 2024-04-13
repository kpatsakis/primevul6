static int __init init_aout_binfmt(void)
{
	return register_binfmt(&aout_format);
}