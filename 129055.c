static int __init check_elf32_binfmt(void)
{
	if (cpu_uses_ia32el()) {
		printk("Please use IA-32 EL for executing IA-32 binaries\n");
		return unregister_binfmt(&elf_format);
	}
	return 0;
}