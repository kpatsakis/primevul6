static int elf_fdpic_map_file_by_direct_mmap(struct elf_fdpic_params *params,
					     struct file *file,
					     struct mm_struct *mm)
{
	struct elf32_fdpic_loadseg *seg;
	struct elf32_phdr *phdr;
	unsigned long load_addr, delta_vaddr;
	int loop, dvset;

	load_addr = params->load_addr;
	delta_vaddr = 0;
	dvset = 0;

	seg = params->loadmap->segs;

	/* deal with each load segment separately */
	phdr = params->phdrs;
	for (loop = 0; loop < params->hdr.e_phnum; loop++, phdr++) {
		unsigned long maddr, disp, excess, excess1;
		int prot = 0, flags;

		if (phdr->p_type != PT_LOAD)
			continue;

		kdebug("[LOAD] va=%lx of=%lx fs=%lx ms=%lx",
		       (unsigned long) phdr->p_vaddr,
		       (unsigned long) phdr->p_offset,
		       (unsigned long) phdr->p_filesz,
		       (unsigned long) phdr->p_memsz);

		/* determine the mapping parameters */
		if (phdr->p_flags & PF_R) prot |= PROT_READ;
		if (phdr->p_flags & PF_W) prot |= PROT_WRITE;
		if (phdr->p_flags & PF_X) prot |= PROT_EXEC;

		flags = MAP_PRIVATE | MAP_DENYWRITE;
		if (params->flags & ELF_FDPIC_FLAG_EXECUTABLE)
			flags |= MAP_EXECUTABLE;

		maddr = 0;

		switch (params->flags & ELF_FDPIC_FLAG_ARRANGEMENT) {
		case ELF_FDPIC_FLAG_INDEPENDENT:
			/* PT_LOADs are independently locatable */
			break;

		case ELF_FDPIC_FLAG_HONOURVADDR:
			/* the specified virtual address must be honoured */
			maddr = phdr->p_vaddr;
			flags |= MAP_FIXED;
			break;

		case ELF_FDPIC_FLAG_CONSTDISP:
			/* constant displacement
			 * - can be mapped anywhere, but must be mapped as a
			 *   unit
			 */
			if (!dvset) {
				maddr = load_addr;
				delta_vaddr = phdr->p_vaddr;
				dvset = 1;
			} else {
				maddr = load_addr + phdr->p_vaddr - delta_vaddr;
				flags |= MAP_FIXED;
			}
			break;

		case ELF_FDPIC_FLAG_CONTIGUOUS:
			/* contiguity handled later */
			break;

		default:
			BUG();
		}

		maddr &= PAGE_MASK;

		/* create the mapping */
		disp = phdr->p_vaddr & ~PAGE_MASK;
		down_write(&mm->mmap_sem);
		maddr = do_mmap(file, maddr, phdr->p_memsz + disp, prot, flags,
				phdr->p_offset - disp);
		up_write(&mm->mmap_sem);

		kdebug("mmap[%d] <file> sz=%lx pr=%x fl=%x of=%lx --> %08lx",
		       loop, phdr->p_memsz + disp, prot, flags,
		       phdr->p_offset - disp, maddr);

		if (IS_ERR_VALUE(maddr))
			return (int) maddr;

		if ((params->flags & ELF_FDPIC_FLAG_ARRANGEMENT) ==
		    ELF_FDPIC_FLAG_CONTIGUOUS)
			load_addr += PAGE_ALIGN(phdr->p_memsz + disp);

		seg->addr = maddr + disp;
		seg->p_vaddr = phdr->p_vaddr;
		seg->p_memsz = phdr->p_memsz;

		/* map the ELF header address if in this segment */
		if (phdr->p_offset == 0)
			params->elfhdr_addr = seg->addr;

		/* clear the bit between beginning of mapping and beginning of
		 * PT_LOAD */
		if (prot & PROT_WRITE && disp > 0) {
			kdebug("clear[%d] ad=%lx sz=%lx", loop, maddr, disp);
			clear_user((void __user *) maddr, disp);
			maddr += disp;
		}

		/* clear any space allocated but not loaded
		 * - on uClinux we can just clear the lot
		 * - on MMU linux we'll get a SIGBUS beyond the last page
		 *   extant in the file
		 */
		excess = phdr->p_memsz - phdr->p_filesz;
		excess1 = PAGE_SIZE - ((maddr + phdr->p_filesz) & ~PAGE_MASK);

#ifdef CONFIG_MMU
		if (excess > excess1) {
			unsigned long xaddr = maddr + phdr->p_filesz + excess1;
			unsigned long xmaddr;

			flags |= MAP_FIXED | MAP_ANONYMOUS;
			down_write(&mm->mmap_sem);
			xmaddr = do_mmap(NULL, xaddr, excess - excess1,
					 prot, flags, 0);
			up_write(&mm->mmap_sem);

			kdebug("mmap[%d] <anon>"
			       " ad=%lx sz=%lx pr=%x fl=%x of=0 --> %08lx",
			       loop, xaddr, excess - excess1, prot, flags,
			       xmaddr);

			if (xmaddr != xaddr)
				return -ENOMEM;
		}

		if (prot & PROT_WRITE && excess1 > 0) {
			kdebug("clear[%d] ad=%lx sz=%lx",
			       loop, maddr + phdr->p_filesz, excess1);
			clear_user((void __user *) maddr + phdr->p_filesz,
				   excess1);
		}

#else
		if (excess > 0) {
			kdebug("clear[%d] ad=%lx sz=%lx",
			       loop, maddr + phdr->p_filesz, excess);
			clear_user((void *) maddr + phdr->p_filesz, excess);
		}
#endif

		if (mm) {
			if (phdr->p_flags & PF_X) {
				if (!mm->start_code) {
					mm->start_code = maddr;
					mm->end_code = maddr + phdr->p_memsz;
				}
			} else if (!mm->start_data) {
				mm->start_data = maddr;
				mm->end_data = maddr + phdr->p_memsz;
			}
		}

		seg++;
	}

	return 0;
}