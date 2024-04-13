static int acct_stack_growth(struct vm_area_struct * vma, unsigned long size, unsigned long grow)
{
	struct mm_struct *mm = vma->vm_mm;
	struct rlimit *rlim = current->signal->rlim;
	unsigned long new_start;

	/* address space limit tests */
	if (!may_expand_vm(mm, grow))
		return -ENOMEM;

	/* Stack limit test */
	if (size > rlim[RLIMIT_STACK].rlim_cur)
		return -ENOMEM;

	/* mlock limit tests */
	if (vma->vm_flags & VM_LOCKED) {
		unsigned long locked;
		unsigned long limit;
		locked = mm->locked_vm + grow;
		limit = rlim[RLIMIT_MEMLOCK].rlim_cur >> PAGE_SHIFT;
		if (locked > limit && !capable(CAP_IPC_LOCK))
			return -ENOMEM;
	}

	/* Check to ensure the stack will not grow into a hugetlb-only region */
	new_start = (vma->vm_flags & VM_GROWSUP) ? vma->vm_start :
			vma->vm_end - size;
	if (is_hugepage_only_range(vma->vm_mm, new_start, size))
		return -EFAULT;

	/*
	 * Overcommit..  This must be the final test, as it will
	 * update security statistics.
	 */
	if (security_vm_enough_memory(grow))
		return -ENOMEM;

	/* Ok, everything looks good - let it rip */
	mm->total_vm += grow;
	if (vma->vm_flags & VM_LOCKED)
		mm->locked_vm += grow;
	vm_stat_account(mm, vma->vm_flags, vma->vm_file, grow);
	return 0;
}