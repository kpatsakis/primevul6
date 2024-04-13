static unsigned long load_aout_interp(struct exec *interp_ex,
		struct file *interpreter)
{
	unsigned long text_data, elf_entry = ~0UL;
	char __user * addr;
	loff_t offset;

	current->mm->end_code = interp_ex->a_text;
	text_data = interp_ex->a_text + interp_ex->a_data;
	current->mm->end_data = text_data;
	current->mm->brk = interp_ex->a_bss + text_data;

	switch (N_MAGIC(*interp_ex)) {
	case OMAGIC:
		offset = 32;
		addr = (char __user *)0;
		break;
	case ZMAGIC:
	case QMAGIC:
		offset = N_TXTOFF(*interp_ex);
		addr = (char __user *)N_TXTADDR(*interp_ex);
		break;
	default:
		goto out;
	}

	down_write(&current->mm->mmap_sem);	
	do_brk(0, text_data);
	up_write(&current->mm->mmap_sem);
	if (!interpreter->f_op || !interpreter->f_op->read)
		goto out;
	if (interpreter->f_op->read(interpreter, addr, text_data, &offset) < 0)
		goto out;
	flush_icache_range((unsigned long)addr,
	                   (unsigned long)addr + text_data);

	down_write(&current->mm->mmap_sem);	
	do_brk(ELF_PAGESTART(text_data + ELF_MIN_ALIGN - 1),
		interp_ex->a_bss);
	up_write(&current->mm->mmap_sem);
	elf_entry = interp_ex->a_entry;

out:
	return elf_entry;
}