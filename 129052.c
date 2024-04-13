static void __exit exit_elf_fdpic_binfmt(void)
{
	unregister_binfmt(&elf_fdpic_format);
}