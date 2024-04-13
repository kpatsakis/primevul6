static int __init init_elf_fdpic_binfmt(void)
{
	return register_binfmt(&elf_fdpic_format);
}