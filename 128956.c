asmlinkage unsigned long sys_mremap(unsigned long addr,
	unsigned long old_len, unsigned long new_len,
	unsigned long flags, unsigned long new_addr)
{
	unsigned long ret;

	down_write(&current->mm->mmap_sem);
	ret = do_mremap(addr, old_len, new_len, flags, new_addr);
	up_write(&current->mm->mmap_sem);
	return ret;
}