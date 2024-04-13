static int elf_fdpic_map_file(struct elf_fdpic_params *params,
			      struct file *file,
			      struct mm_struct *mm,
			      const char *what)
{
	struct elf32_fdpic_loadmap *loadmap;
#ifdef CONFIG_MMU
	struct elf32_fdpic_loadseg *mseg;
#endif
	struct elf32_fdpic_loadseg *seg;
	struct elf32_phdr *phdr;
	unsigned long load_addr, stop;
	unsigned nloads, tmp;
	size_t size;
	int loop, ret;

	/* allocate a load map table */
	nloads = 0;
	for (loop = 0; loop < params->hdr.e_phnum; loop++)
		if (params->phdrs[loop].p_type == PT_LOAD)
			nloads++;

	if (nloads == 0)
		return -ELIBBAD;

	size = sizeof(*loadmap) + nloads * sizeof(*seg);
	loadmap = kzalloc(size, GFP_KERNEL);
	if (!loadmap)
		return -ENOMEM;

	params->loadmap = loadmap;

	loadmap->version = ELF32_FDPIC_LOADMAP_VERSION;
	loadmap->nsegs = nloads;

	load_addr = params->load_addr;
	seg = loadmap->segs;

	/* map the requested LOADs into the memory space */
	switch (params->flags & ELF_FDPIC_FLAG_ARRANGEMENT) {
	case ELF_FDPIC_FLAG_CONSTDISP:
	case ELF_FDPIC_FLAG_CONTIGUOUS:
#ifndef CONFIG_MMU
		ret = elf_fdpic_map_file_constdisp_on_uclinux(params, file, mm);
		if (ret < 0)
			return ret;
		break;
#endif
	default:
		ret = elf_fdpic_map_file_by_direct_mmap(params, file, mm);
		if (ret < 0)
			return ret;
		break;
	}

	/* map the entry point */
	if (params->hdr.e_entry) {
		seg = loadmap->segs;
		for (loop = loadmap->nsegs; loop > 0; loop--, seg++) {
			if (params->hdr.e_entry >= seg->p_vaddr &&
			    params->hdr.e_entry < seg->p_vaddr + seg->p_memsz) {
				params->entry_addr =
					(params->hdr.e_entry - seg->p_vaddr) +
					seg->addr;
				break;
			}
		}
	}

	/* determine where the program header table has wound up if mapped */
	stop = params->hdr.e_phoff;
	stop += params->hdr.e_phnum * sizeof (struct elf_phdr);
	phdr = params->phdrs;

	for (loop = 0; loop < params->hdr.e_phnum; loop++, phdr++) {
		if (phdr->p_type != PT_LOAD)
			continue;

		if (phdr->p_offset > params->hdr.e_phoff ||
		    phdr->p_offset + phdr->p_filesz < stop)
			continue;

		seg = loadmap->segs;
		for (loop = loadmap->nsegs; loop > 0; loop--, seg++) {
			if (phdr->p_vaddr >= seg->p_vaddr &&
			    phdr->p_vaddr + phdr->p_filesz <=
			    seg->p_vaddr + seg->p_memsz) {
				params->ph_addr =
					(phdr->p_vaddr - seg->p_vaddr) +
					seg->addr +
					params->hdr.e_phoff - phdr->p_offset;
				break;
			}
		}
		break;
	}

	/* determine where the dynamic section has wound up if there is one */
	phdr = params->phdrs;
	for (loop = 0; loop < params->hdr.e_phnum; loop++, phdr++) {
		if (phdr->p_type != PT_DYNAMIC)
			continue;

		seg = loadmap->segs;
		for (loop = loadmap->nsegs; loop > 0; loop--, seg++) {
			if (phdr->p_vaddr >= seg->p_vaddr &&
			    phdr->p_vaddr + phdr->p_memsz <=
			    seg->p_vaddr + seg->p_memsz) {
				params->dynamic_addr =
					(phdr->p_vaddr - seg->p_vaddr) +
					seg->addr;

				/* check the dynamic section contains at least
				 * one item, and that the last item is a NULL
				 * entry */
				if (phdr->p_memsz == 0 ||
				    phdr->p_memsz % sizeof(Elf32_Dyn) != 0)
					goto dynamic_error;

				tmp = phdr->p_memsz / sizeof(Elf32_Dyn);
				if (((Elf32_Dyn *)
				     params->dynamic_addr)[tmp - 1].d_tag != 0)
					goto dynamic_error;
				break;
			}
		}
		break;
	}

	/* now elide adjacent segments in the load map on MMU linux
	 * - on uClinux the holes between may actually be filled with system
	 *   stuff or stuff from other processes
	 */
#ifdef CONFIG_MMU
	nloads = loadmap->nsegs;
	mseg = loadmap->segs;
	seg = mseg + 1;
	for (loop = 1; loop < nloads; loop++) {
		/* see if we have a candidate for merging */
		if (seg->p_vaddr - mseg->p_vaddr == seg->addr - mseg->addr) {
			load_addr = PAGE_ALIGN(mseg->addr + mseg->p_memsz);
			if (load_addr == (seg->addr & PAGE_MASK)) {
				mseg->p_memsz +=
					load_addr -
					(mseg->addr + mseg->p_memsz);
				mseg->p_memsz += seg->addr & ~PAGE_MASK;
				mseg->p_memsz += seg->p_memsz;
				loadmap->nsegs--;
				continue;
			}
		}

		mseg++;
		if (mseg != seg)
			*mseg = *seg;
	}
#endif

	kdebug("Mapped Object [%s]:", what);
	kdebug("- elfhdr   : %lx", params->elfhdr_addr);
	kdebug("- entry    : %lx", params->entry_addr);
	kdebug("- PHDR[]   : %lx", params->ph_addr);
	kdebug("- DYNAMIC[]: %lx", params->dynamic_addr);
	seg = loadmap->segs;
	for (loop = 0; loop < loadmap->nsegs; loop++, seg++)
		kdebug("- LOAD[%d] : %08x-%08x [va=%x ms=%x]",
		       loop,
		       seg->addr, seg->addr + seg->p_memsz - 1,
		       seg->p_vaddr, seg->p_memsz);

	return 0;

dynamic_error:
	printk("ELF FDPIC %s with invalid DYNAMIC section (inode=%lu)\n",
	       what, file->f_path.dentry->d_inode->i_ino);
	return -ELIBBAD;
}