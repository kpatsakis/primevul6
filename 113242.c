static inline int fixup_user_fault(struct task_struct *tsk,
		struct mm_struct *mm, unsigned long address,
		unsigned int fault_flags, bool *unlocked)
{
	/* should never happen if there's no MMU */
	BUG();
	return -EFAULT;
}