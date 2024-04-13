__elf_is_okay__(Elf_Ehdr *ehdr)
{
	int retval = 0;
	/*
	 * We need to check magic, class size, endianess,
	 * and version before we look at the rest of the
	 * Elf_Ehdr structure.  These few elements are
	 * represented in a machine independent fashion.
	 */
	if (IS_ELF(*ehdr) &&
	    ehdr->e_ident[EI_CLASS] == ELF_TARG_CLASS &&
	    ehdr->e_ident[EI_DATA] == ELF_TARG_DATA &&
	    ehdr->e_ident[EI_VERSION] == ELF_TARG_VER) {

		/* Now check the machine dependent header */
		if (ehdr->e_machine == ELF_TARG_MACH &&
		    ehdr->e_version == ELF_TARG_VER)
			retval = 1;
	}
	return retval;
}