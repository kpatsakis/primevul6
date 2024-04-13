static void set_brk(unsigned long start, unsigned long end)
{
	start = PAGE_ALIGN(start);
	end = PAGE_ALIGN(end);
	if (end <= start)
		return;
	down_write(&current->mm->mmap_sem);
	do_brk(start, end - start);
	up_write(&current->mm->mmap_sem);
}