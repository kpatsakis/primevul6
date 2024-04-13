static void __exit exit_aout_binfmt(void)
{
	unregister_binfmt(&aout_format);
}