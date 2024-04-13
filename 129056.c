int may_expand_vm(struct mm_struct *mm, unsigned long npages)
{
	unsigned long cur = mm->total_vm;	/* pages */
	unsigned long lim;

	lim = current->signal->rlim[RLIMIT_AS].rlim_cur >> PAGE_SHIFT;

	if (cur + npages > lim)
		return 0;
	return 1;
}