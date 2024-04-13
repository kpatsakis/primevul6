static int __init init_elf_binfmt(void)
{
	return register_binfmt(&elf_format);
}