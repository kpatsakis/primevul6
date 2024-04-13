static int elf_core_dump(long signr, struct pt_regs *regs, struct file *file)
{
#define	NUM_NOTES	6
	int has_dumped = 0;
	mm_segment_t fs;
	int segs;
	size_t size = 0;
	int i;
	struct vm_area_struct *vma, *gate_vma;
	struct elfhdr *elf = NULL;
	loff_t offset = 0, dataoff, foffset;
	unsigned long limit = current->signal->rlim[RLIMIT_CORE].rlim_cur;
	int numnote;
	struct memelfnote *notes = NULL;
	struct elf_prstatus *prstatus = NULL;	/* NT_PRSTATUS */
	struct elf_prpsinfo *psinfo = NULL;	/* NT_PRPSINFO */
 	struct task_struct *g, *p;
 	LIST_HEAD(thread_list);
 	struct list_head *t;
	elf_fpregset_t *fpu = NULL;
#ifdef ELF_CORE_COPY_XFPREGS
	elf_fpxregset_t *xfpu = NULL;
#endif
	int thread_status_size = 0;
	elf_addr_t *auxv;
#ifdef ELF_CORE_WRITE_EXTRA_NOTES
	int extra_notes_size;
#endif

	/*
	 * We no longer stop all VM operations.
	 * 
	 * This is because those proceses that could possibly change map_count
	 * or the mmap / vma pages are now blocked in do_exit on current
	 * finishing this core dump.
	 *
	 * Only ptrace can touch these memory addresses, but it doesn't change
	 * the map_count or the pages allocated. So no possibility of crashing
	 * exists while dumping the mm->vm_next areas to the core file.
	 */
  
	/* alloc memory for large data structures: too large to be on stack */
	elf = kmalloc(sizeof(*elf), GFP_KERNEL);
	if (!elf)
		goto cleanup;
	prstatus = kmalloc(sizeof(*prstatus), GFP_KERNEL);
	if (!prstatus)
		goto cleanup;
	psinfo = kmalloc(sizeof(*psinfo), GFP_KERNEL);
	if (!psinfo)
		goto cleanup;
	notes = kmalloc(NUM_NOTES * sizeof(struct memelfnote), GFP_KERNEL);
	if (!notes)
		goto cleanup;
	fpu = kmalloc(sizeof(*fpu), GFP_KERNEL);
	if (!fpu)
		goto cleanup;
#ifdef ELF_CORE_COPY_XFPREGS
	xfpu = kmalloc(sizeof(*xfpu), GFP_KERNEL);
	if (!xfpu)
		goto cleanup;
#endif

	if (signr) {
		struct elf_thread_status *tmp;
		rcu_read_lock();
		do_each_thread(g,p)
			if (current->mm == p->mm && current != p) {
				tmp = kzalloc(sizeof(*tmp), GFP_ATOMIC);
				if (!tmp) {
					rcu_read_unlock();
					goto cleanup;
				}
				tmp->thread = p;
				list_add(&tmp->list, &thread_list);
			}
		while_each_thread(g,p);
		rcu_read_unlock();
		list_for_each(t, &thread_list) {
			struct elf_thread_status *tmp;
			int sz;

			tmp = list_entry(t, struct elf_thread_status, list);
			sz = elf_dump_thread_status(signr, tmp);
			thread_status_size += sz;
		}
	}
	/* now collect the dump for the current */
	memset(prstatus, 0, sizeof(*prstatus));
	fill_prstatus(prstatus, current, signr);
	elf_core_copy_regs(&prstatus->pr_reg, regs);
	
	segs = current->mm->map_count;
#ifdef ELF_CORE_EXTRA_PHDRS
	segs += ELF_CORE_EXTRA_PHDRS;
#endif

	gate_vma = get_gate_vma(current);
	if (gate_vma != NULL)
		segs++;

	/* Set up header */
	fill_elf_header(elf, segs + 1);	/* including notes section */

	has_dumped = 1;
	current->flags |= PF_DUMPCORE;

	/*
	 * Set up the notes in similar form to SVR4 core dumps made
	 * with info from their /proc.
	 */

	fill_note(notes + 0, "CORE", NT_PRSTATUS, sizeof(*prstatus), prstatus);
	fill_psinfo(psinfo, current->group_leader, current->mm);
	fill_note(notes + 1, "CORE", NT_PRPSINFO, sizeof(*psinfo), psinfo);
	
	numnote = 2;

	auxv = (elf_addr_t *)current->mm->saved_auxv;

	i = 0;
	do
		i += 2;
	while (auxv[i - 2] != AT_NULL);
	fill_note(&notes[numnote++], "CORE", NT_AUXV,
		  i * sizeof(elf_addr_t), auxv);

  	/* Try to dump the FPU. */
	if ((prstatus->pr_fpvalid =
	     elf_core_copy_task_fpregs(current, regs, fpu)))
		fill_note(notes + numnote++,
			  "CORE", NT_PRFPREG, sizeof(*fpu), fpu);
#ifdef ELF_CORE_COPY_XFPREGS
	if (elf_core_copy_task_xfpregs(current, xfpu))
		fill_note(notes + numnote++,
			  "LINUX", NT_PRXFPREG, sizeof(*xfpu), xfpu);
#endif	
  
	fs = get_fs();
	set_fs(KERNEL_DS);

	DUMP_WRITE(elf, sizeof(*elf));
	offset += sizeof(*elf);				/* Elf header */
	offset += (segs + 1) * sizeof(struct elf_phdr); /* Program headers */
	foffset = offset;

	/* Write notes phdr entry */
	{
		struct elf_phdr phdr;
		int sz = 0;

		for (i = 0; i < numnote; i++)
			sz += notesize(notes + i);
		
		sz += thread_status_size;

#ifdef ELF_CORE_WRITE_EXTRA_NOTES
		extra_notes_size = ELF_CORE_EXTRA_NOTES_SIZE;
		sz += extra_notes_size;
#endif

		fill_elf_note_phdr(&phdr, sz, offset);
		offset += sz;
		DUMP_WRITE(&phdr, sizeof(phdr));
	}

	dataoff = offset = roundup(offset, ELF_EXEC_PAGESIZE);

	/* Write program headers for segments dump */
	for (vma = first_vma(current, gate_vma); vma != NULL;
			vma = next_vma(vma, gate_vma)) {
		struct elf_phdr phdr;
		size_t sz;

		sz = vma->vm_end - vma->vm_start;

		phdr.p_type = PT_LOAD;
		phdr.p_offset = offset;
		phdr.p_vaddr = vma->vm_start;
		phdr.p_paddr = 0;
		phdr.p_filesz = maydump(vma) ? sz : 0;
		phdr.p_memsz = sz;
		offset += phdr.p_filesz;
		phdr.p_flags = vma->vm_flags & VM_READ ? PF_R : 0;
		if (vma->vm_flags & VM_WRITE)
			phdr.p_flags |= PF_W;
		if (vma->vm_flags & VM_EXEC)
			phdr.p_flags |= PF_X;
		phdr.p_align = ELF_EXEC_PAGESIZE;

		DUMP_WRITE(&phdr, sizeof(phdr));
	}

#ifdef ELF_CORE_WRITE_EXTRA_PHDRS
	ELF_CORE_WRITE_EXTRA_PHDRS;
#endif

 	/* write out the notes section */
	for (i = 0; i < numnote; i++)
		if (!writenote(notes + i, file, &foffset))
			goto end_coredump;

#ifdef ELF_CORE_WRITE_EXTRA_NOTES
	ELF_CORE_WRITE_EXTRA_NOTES;
	foffset += extra_notes_size;
#endif

	/* write out the thread status notes section */
	list_for_each(t, &thread_list) {
		struct elf_thread_status *tmp =
				list_entry(t, struct elf_thread_status, list);

		for (i = 0; i < tmp->num_notes; i++)
			if (!writenote(&tmp->notes[i], file, &foffset))
				goto end_coredump;
	}

	/* Align to page */
	DUMP_SEEK(dataoff - foffset);

	for (vma = first_vma(current, gate_vma); vma != NULL;
			vma = next_vma(vma, gate_vma)) {
		unsigned long addr;

		if (!maydump(vma))
			continue;

		for (addr = vma->vm_start;
		     addr < vma->vm_end;
		     addr += PAGE_SIZE) {
			struct page *page;
			struct vm_area_struct *vma;

			if (get_user_pages(current, current->mm, addr, 1, 0, 1,
						&page, &vma) <= 0) {
				DUMP_SEEK(PAGE_SIZE);
			} else {
				if (page == ZERO_PAGE(addr)) {
					if (!dump_seek(file, PAGE_SIZE)) {
						page_cache_release(page);
						goto end_coredump;
					}
				} else {
					void *kaddr;
					flush_cache_page(vma, addr,
							 page_to_pfn(page));
					kaddr = kmap(page);
					if ((size += PAGE_SIZE) > limit ||
					    !dump_write(file, kaddr,
					    PAGE_SIZE)) {
						kunmap(page);
						page_cache_release(page);
						goto end_coredump;
					}
					kunmap(page);
				}
				page_cache_release(page);
			}
		}
	}

#ifdef ELF_CORE_WRITE_EXTRA_DATA
	ELF_CORE_WRITE_EXTRA_DATA;
#endif

end_coredump:
	set_fs(fs);

cleanup:
	while (!list_empty(&thread_list)) {
		struct list_head *tmp = thread_list.next;
		list_del(tmp);
		kfree(list_entry(tmp, struct elf_thread_status, list));
	}

	kfree(elf);
	kfree(prstatus);
	kfree(psinfo);
	kfree(notes);
	kfree(fpu);
#ifdef ELF_CORE_COPY_XFPREGS
	kfree(xfpu);
#endif
	return has_dumped;
#undef NUM_NOTES
}