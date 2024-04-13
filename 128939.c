static int __init init_script_binfmt(void)
{
	return register_binfmt(&script_format);
}