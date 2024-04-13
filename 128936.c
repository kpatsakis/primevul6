static inline void fill_elf_fdpic_header(struct elfhdr *elf, int segs)
{
	memcpy(elf->e_ident, ELFMAG, SELFMAG);
	elf->e_ident[EI_CLASS] = ELF_CLASS;
	elf->e_ident[EI_DATA] = ELF_DATA;
	elf->e_ident[EI_VERSION] = EV_CURRENT;
	elf->e_ident[EI_OSABI] = ELF_OSABI;
	memset(elf->e_ident+EI_PAD, 0, EI_NIDENT-EI_PAD);

	elf->e_type = ET_CORE;
	elf->e_machine = ELF_ARCH;
	elf->e_version = EV_CURRENT;
	elf->e_entry = 0;
	elf->e_phoff = sizeof(struct elfhdr);
	elf->e_shoff = 0;
	elf->e_flags = ELF_FDPIC_CORE_EFLAGS;
	elf->e_ehsize = sizeof(struct elfhdr);
	elf->e_phentsize = sizeof(struct elf_phdr);
	elf->e_phnum = segs;
	elf->e_shentsize = 0;
	elf->e_shnum = 0;
	elf->e_shstrndx = 0;
	return;
}