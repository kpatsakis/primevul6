static int set_brk(unsigned long start, unsigned long end)
{
	start = ELF_PAGEALIGN(start);
	end = ELF_PAGEALIGN(end);
	if (end > start) {
		unsigned long addr;
		down_write(&current->mm->mmap_sem);
		addr = do_brk(start, end - start);
		up_write(&current->mm->mmap_sem);
		if (BAD_ADDR(addr))
			return addr;
	}
	current->mm->start_brk = current->mm->brk = end;
	return 0;
}