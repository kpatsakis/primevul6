elf32_map (struct file *filep, unsigned long addr, struct elf_phdr *eppnt, int prot, int type, unsigned long unused)
{
	unsigned long pgoff = (eppnt->p_vaddr) & ~IA32_PAGE_MASK;

	return ia32_do_mmap(filep, (addr & IA32_PAGE_MASK), eppnt->p_filesz + pgoff, prot, type,
			    eppnt->p_offset - pgoff);
}