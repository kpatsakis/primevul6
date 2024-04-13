static int elf_fdpic_fetch_phdrs(struct elf_fdpic_params *params,
				 struct file *file)
{
	struct elf32_phdr *phdr;
	unsigned long size;
	int retval, loop;

	if (params->hdr.e_phentsize != sizeof(struct elf_phdr))
		return -ENOMEM;
	if (params->hdr.e_phnum > 65536U / sizeof(struct elf_phdr))
		return -ENOMEM;

	size = params->hdr.e_phnum * sizeof(struct elf_phdr);
	params->phdrs = kmalloc(size, GFP_KERNEL);
	if (!params->phdrs)
		return -ENOMEM;

	retval = kernel_read(file, params->hdr.e_phoff,
			     (char *) params->phdrs, size);
	if (retval < 0)
		return retval;

	/* determine stack size for this binary */
	phdr = params->phdrs;
	for (loop = 0; loop < params->hdr.e_phnum; loop++, phdr++) {
		if (phdr->p_type != PT_GNU_STACK)
			continue;

		if (phdr->p_flags & PF_X)
			params->flags |= ELF_FDPIC_FLAG_EXEC_STACK;
		else
			params->flags |= ELF_FDPIC_FLAG_NOEXEC_STACK;

		params->stack_size = phdr->p_memsz;
		break;
	}

	return 0;
}