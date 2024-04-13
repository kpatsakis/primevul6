int setup_arg_pages(struct linux_binprm *bprm,
		    unsigned long stack_top,
		    int executable_stack)
{
	unsigned long ret;
	unsigned long stack_shift;
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma = bprm->vma;
	struct vm_area_struct *prev = NULL;
	unsigned long vm_flags;
	unsigned long stack_base;

#ifdef CONFIG_STACK_GROWSUP
	/* Limit stack size to 1GB */
	stack_base = current->signal->rlim[RLIMIT_STACK].rlim_max;
	if (stack_base > (1 << 30))
		stack_base = 1 << 30;

	/* Make sure we didn't let the argument array grow too large. */
	if (vma->vm_end - vma->vm_start > stack_base)
		return -ENOMEM;

	stack_base = PAGE_ALIGN(stack_top - stack_base);

	stack_shift = vma->vm_start - stack_base;
	mm->arg_start = bprm->p - stack_shift;
	bprm->p = vma->vm_end - stack_shift;
#else
	stack_top = arch_align_stack(stack_top);
	stack_top = PAGE_ALIGN(stack_top);
	stack_shift = vma->vm_end - stack_top;

	bprm->p -= stack_shift;
	mm->arg_start = bprm->p;
#endif

	if (bprm->loader)
		bprm->loader -= stack_shift;
	bprm->exec -= stack_shift;

	down_write(&mm->mmap_sem);
	vm_flags = vma->vm_flags;

	/*
	 * Adjust stack execute permissions; explicitly enable for
	 * EXSTACK_ENABLE_X, disable for EXSTACK_DISABLE_X and leave alone
	 * (arch default) otherwise.
	 */
	if (unlikely(executable_stack == EXSTACK_ENABLE_X))
		vm_flags |= VM_EXEC;
	else if (executable_stack == EXSTACK_DISABLE_X)
		vm_flags &= ~VM_EXEC;
	vm_flags |= mm->def_flags;

	ret = mprotect_fixup(vma, &prev, vma->vm_start, vma->vm_end,
			vm_flags);
	if (ret)
		goto out_unlock;
	BUG_ON(prev != vma);

	/* Move stack pages down in memory. */
	if (stack_shift) {
		ret = shift_arg_pages(vma, stack_shift);
		if (ret) {
			up_write(&mm->mmap_sem);
			return ret;
		}
	}

#ifdef CONFIG_STACK_GROWSUP
	stack_base = vma->vm_end + EXTRA_STACK_VM_PAGES * PAGE_SIZE;
#else
	stack_base = vma->vm_start - EXTRA_STACK_VM_PAGES * PAGE_SIZE;
#endif
	ret = expand_stack(vma, stack_base);
	if (ret)
		ret = -EFAULT;

out_unlock:
	up_write(&mm->mmap_sem);
	return 0;
}