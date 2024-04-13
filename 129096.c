ia64_elf32_init (struct pt_regs *regs)
{
	struct vm_area_struct *vma;

	/*
	 * Map GDT below 4GB, where the processor can find it.  We need to map
	 * it with privilege level 3 because the IVE uses non-privileged accesses to these
	 * tables.  IA-32 segmentation is used to protect against IA-32 accesses to them.
	 */
	vma = kmem_cache_zalloc(vm_area_cachep, GFP_KERNEL);
	if (vma) {
		vma->vm_mm = current->mm;
		vma->vm_start = IA32_GDT_OFFSET;
		vma->vm_end = vma->vm_start + PAGE_SIZE;
		vma->vm_page_prot = PAGE_SHARED;
		vma->vm_flags = VM_READ|VM_MAYREAD|VM_RESERVED;
		vma->vm_ops = &ia32_shared_page_vm_ops;
		down_write(&current->mm->mmap_sem);
		{
			if (insert_vm_struct(current->mm, vma)) {
				kmem_cache_free(vm_area_cachep, vma);
				up_write(&current->mm->mmap_sem);
				BUG();
			}
		}
		up_write(&current->mm->mmap_sem);
	}

	/*
	 * When user stack is not executable, push sigreturn code to stack makes
	 * segmentation fault raised when returning to kernel. So now sigreturn
	 * code is locked in specific gate page, which is pointed by pretcode
	 * when setup_frame_ia32
	 */
	vma = kmem_cache_zalloc(vm_area_cachep, GFP_KERNEL);
	if (vma) {
		vma->vm_mm = current->mm;
		vma->vm_start = IA32_GATE_OFFSET;
		vma->vm_end = vma->vm_start + PAGE_SIZE;
		vma->vm_page_prot = PAGE_COPY_EXEC;
		vma->vm_flags = VM_READ | VM_MAYREAD | VM_EXEC
				| VM_MAYEXEC | VM_RESERVED;
		vma->vm_ops = &ia32_gate_page_vm_ops;
		down_write(&current->mm->mmap_sem);
		{
			if (insert_vm_struct(current->mm, vma)) {
				kmem_cache_free(vm_area_cachep, vma);
				up_write(&current->mm->mmap_sem);
				BUG();
			}
		}
		up_write(&current->mm->mmap_sem);
	}

	/*
	 * Install LDT as anonymous memory.  This gives us all-zero segment descriptors
	 * until a task modifies them via modify_ldt().
	 */
	vma = kmem_cache_zalloc(vm_area_cachep, GFP_KERNEL);
	if (vma) {
		vma->vm_mm = current->mm;
		vma->vm_start = IA32_LDT_OFFSET;
		vma->vm_end = vma->vm_start + PAGE_ALIGN(IA32_LDT_ENTRIES*IA32_LDT_ENTRY_SIZE);
		vma->vm_page_prot = PAGE_SHARED;
		vma->vm_flags = VM_READ|VM_WRITE|VM_MAYREAD|VM_MAYWRITE;
		down_write(&current->mm->mmap_sem);
		{
			if (insert_vm_struct(current->mm, vma)) {
				kmem_cache_free(vm_area_cachep, vma);
				up_write(&current->mm->mmap_sem);
				BUG();
			}
		}
		up_write(&current->mm->mmap_sem);
	}

	ia64_psr(regs)->ac = 0;		/* turn off alignment checking */
	regs->loadrs = 0;
	/*
	 *  According to the ABI %edx points to an `atexit' handler.  Since we don't have
	 *  one we'll set it to 0 and initialize all the other registers just to make
	 *  things more deterministic, ala the i386 implementation.
	 */
	regs->r8 = 0;	/* %eax */
	regs->r11 = 0;	/* %ebx */
	regs->r9 = 0;	/* %ecx */
	regs->r10 = 0;	/* %edx */
	regs->r13 = 0;	/* %ebp */
	regs->r14 = 0;	/* %esi */
	regs->r15 = 0;	/* %edi */

	current->thread.eflag = IA32_EFLAG;
	current->thread.fsr = IA32_FSR_DEFAULT;
	current->thread.fcr = IA32_FCR_DEFAULT;
	current->thread.fir = 0;
	current->thread.fdr = 0;

	/*
	 * Setup GDTD.  Note: GDTD is the descrambled version of the pseudo-descriptor
	 * format defined by Figure 3-11 "Pseudo-Descriptor Format" in the IA-32
	 * architecture manual. Also note that the only fields that are not ignored are
	 * `base', `limit', 'G', `P' (must be 1) and `S' (must be 0).
	 */
	regs->r31 = IA32_SEG_UNSCRAMBLE(IA32_SEG_DESCRIPTOR(IA32_GDT_OFFSET, IA32_PAGE_SIZE - 1,
							    0, 0, 0, 1, 0, 0, 0));
	/* Setup the segment selectors */
	regs->r16 = (__USER_DS << 16) | __USER_DS; /* ES == DS, GS, FS are zero */
	regs->r17 = (__USER_DS << 16) | __USER_CS; /* SS, CS; ia32_load_state() sets TSS and LDT */

	ia32_load_segment_descriptors(current);
	ia32_load_state(current);
}