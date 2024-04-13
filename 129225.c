static int elf_fdpic_transfer_args_to_stack(struct linux_binprm *bprm,
					    unsigned long *_sp)
{
	unsigned long index, stop, sp;
	char *src;
	int ret = 0;

	stop = bprm->p >> PAGE_SHIFT;
	sp = *_sp;

	for (index = MAX_ARG_PAGES - 1; index >= stop; index--) {
		src = kmap(bprm->page[index]);
		sp -= PAGE_SIZE;
		if (copy_to_user((void *) sp, src, PAGE_SIZE) != 0)
			ret = -EFAULT;
		kunmap(bprm->page[index]);
		if (ret < 0)
			goto out;
	}

	*_sp = (*_sp - (MAX_ARG_PAGES * PAGE_SIZE - bprm->p)) & ~15;

out:
	return ret;
}