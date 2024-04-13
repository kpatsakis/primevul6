static int unsafe_exec(struct task_struct *p)
{
	int unsafe = 0;
	if (p->ptrace & PT_PTRACED) {
		if (p->ptrace & PT_PTRACE_CAP)
			unsafe |= LSM_UNSAFE_PTRACE_CAP;
		else
			unsafe |= LSM_UNSAFE_PTRACE;
	}
	if (atomic_read(&p->fs->count) > 1 ||
	    atomic_read(&p->files->count) > 1 ||
	    atomic_read(&p->sighand->count) > 1)
		unsafe |= LSM_UNSAFE_SHARE;

	return unsafe;
}