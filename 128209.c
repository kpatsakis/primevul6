elf_sym_to_nlist(struct nlist *nl, Elf_Sym *s, Elf_Shdr *shdr, int shnum)
{
	nl->n_value = s->st_value;

	switch (s->st_shndx) {
	case SHN_UNDEF:
	case SHN_COMMON:
		nl->n_type = N_UNDF;
		break;
	case SHN_ABS:
		nl->n_type = ELF_ST_TYPE(s->st_info) == STT_FILE ?
		    N_FN : N_ABS;
		break;
	default:
		if (s->st_shndx >= shnum)
			nl->n_type = N_UNDF;
		else {
			Elf_Shdr *sh = shdr + s->st_shndx;

			nl->n_type = sh->sh_type == SHT_PROGBITS ?
			    (sh->sh_flags & SHF_WRITE ? N_DATA : N_TEXT) :
			    (sh->sh_type == SHT_NOBITS ? N_BSS : N_UNDF);
		}
		break;
	}

	if (ELF_ST_BIND(s->st_info) == STB_GLOBAL ||
	    ELF_ST_BIND(s->st_info) == STB_WEAK)
		nl->n_type |= N_EXT;
}