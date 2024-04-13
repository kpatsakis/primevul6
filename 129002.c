static int load_aout_binary(struct linux_binprm * bprm, struct pt_regs * regs)
{
	struct exec ex;
	unsigned long error;
	unsigned long fd_offset;
	unsigned long rlim;
	int retval;

	ex = *((struct exec *) bprm->buf);		/* exec-header */
	if ((N_MAGIC(ex) != ZMAGIC && N_MAGIC(ex) != OMAGIC &&
	     N_MAGIC(ex) != QMAGIC && N_MAGIC(ex) != NMAGIC) ||
	    N_TRSIZE(ex) || N_DRSIZE(ex) ||
	    i_size_read(bprm->file->f_path.dentry->d_inode) < ex.a_text+ex.a_data+N_SYMSIZE(ex)+N_TXTOFF(ex)) {
		return -ENOEXEC;
	}

	fd_offset = N_TXTOFF(ex);

	/* Check initial limits. This avoids letting people circumvent
	 * size limits imposed on them by creating programs with large
	 * arrays in the data or bss.
	 */
	rlim = current->signal->rlim[RLIMIT_DATA].rlim_cur;
	if (rlim >= RLIM_INFINITY)
		rlim = ~0;
	if (ex.a_data + ex.a_bss > rlim)
		return -ENOMEM;

	/* Flush all traces of the currently running executable */
	retval = flush_old_exec(bprm);
	if (retval)
		return retval;

	regs->cs = __USER32_CS; 
	regs->r8 = regs->r9 = regs->r10 = regs->r11 = regs->r12 =
		regs->r13 = regs->r14 = regs->r15 = 0;

	/* OK, This is the point of no return */
	set_personality(PER_LINUX);
	set_thread_flag(TIF_IA32); 
	clear_thread_flag(TIF_ABI_PENDING);

	current->mm->end_code = ex.a_text +
		(current->mm->start_code = N_TXTADDR(ex));
	current->mm->end_data = ex.a_data +
		(current->mm->start_data = N_DATADDR(ex));
	current->mm->brk = ex.a_bss +
		(current->mm->start_brk = N_BSSADDR(ex));
	current->mm->free_area_cache = TASK_UNMAPPED_BASE;
	current->mm->cached_hole_size = 0;

	current->mm->mmap = NULL;
	compute_creds(bprm);
 	current->flags &= ~PF_FORKNOEXEC;

	if (N_MAGIC(ex) == OMAGIC) {
		unsigned long text_addr, map_size;
		loff_t pos;

		text_addr = N_TXTADDR(ex);

		pos = 32;
		map_size = ex.a_text+ex.a_data;

		down_write(&current->mm->mmap_sem);
		error = do_brk(text_addr & PAGE_MASK, map_size);
		up_write(&current->mm->mmap_sem);

		if (error != (text_addr & PAGE_MASK)) {
			send_sig(SIGKILL, current, 0);
			return error;
		}

		error = bprm->file->f_op->read(bprm->file,
			 (char __user *)text_addr,
			  ex.a_text+ex.a_data, &pos);
		if ((signed long)error < 0) {
			send_sig(SIGKILL, current, 0);
			return error;
		}
			 
		flush_icache_range(text_addr, text_addr+ex.a_text+ex.a_data);
	} else {
#ifdef WARN_OLD
		static unsigned long error_time, error_time2;
		if ((ex.a_text & 0xfff || ex.a_data & 0xfff) &&
		    (N_MAGIC(ex) != NMAGIC) && (jiffies-error_time2) > 5*HZ)
		{
			printk(KERN_NOTICE "executable not page aligned\n");
			error_time2 = jiffies;
		}

		if ((fd_offset & ~PAGE_MASK) != 0 &&
		    (jiffies-error_time) > 5*HZ)
		{
			printk(KERN_WARNING 
			       "fd_offset is not page aligned. Please convert program: %s\n",
			       bprm->file->f_path.dentry->d_name.name);
			error_time = jiffies;
		}
#endif

		if (!bprm->file->f_op->mmap||((fd_offset & ~PAGE_MASK) != 0)) {
			loff_t pos = fd_offset;
			down_write(&current->mm->mmap_sem);
			do_brk(N_TXTADDR(ex), ex.a_text+ex.a_data);
			up_write(&current->mm->mmap_sem);
			bprm->file->f_op->read(bprm->file,
					(char __user *)N_TXTADDR(ex),
					ex.a_text+ex.a_data, &pos);
			flush_icache_range((unsigned long) N_TXTADDR(ex),
					   (unsigned long) N_TXTADDR(ex) +
					   ex.a_text+ex.a_data);
			goto beyond_if;
		}

		down_write(&current->mm->mmap_sem);
		error = do_mmap(bprm->file, N_TXTADDR(ex), ex.a_text,
			PROT_READ | PROT_EXEC,
			MAP_FIXED | MAP_PRIVATE | MAP_DENYWRITE | MAP_EXECUTABLE | MAP_32BIT,
			fd_offset);
		up_write(&current->mm->mmap_sem);

		if (error != N_TXTADDR(ex)) {
			send_sig(SIGKILL, current, 0);
			return error;
		}

		down_write(&current->mm->mmap_sem);
 		error = do_mmap(bprm->file, N_DATADDR(ex), ex.a_data,
				PROT_READ | PROT_WRITE | PROT_EXEC,
				MAP_FIXED | MAP_PRIVATE | MAP_DENYWRITE | MAP_EXECUTABLE | MAP_32BIT,
				fd_offset + ex.a_text);
		up_write(&current->mm->mmap_sem);
		if (error != N_DATADDR(ex)) {
			send_sig(SIGKILL, current, 0);
			return error;
		}
	}
beyond_if:
	set_binfmt(&aout_format);

	set_brk(current->mm->start_brk, current->mm->brk);

	retval = setup_arg_pages(bprm, IA32_STACK_TOP, EXSTACK_DEFAULT);
	if (retval < 0) { 
		/* Someone check-me: is this error path enough? */ 
		send_sig(SIGKILL, current, 0); 
		return retval;
	}

	current->mm->start_stack =
		(unsigned long)create_aout_tables((char __user *)bprm->p, bprm);
	/* start thread */
	asm volatile("movl %0,%%fs" :: "r" (0)); \
	asm volatile("movl %0,%%es; movl %0,%%ds": :"r" (__USER32_DS));
	load_gs_index(0); 
	(regs)->rip = ex.a_entry;
	(regs)->rsp = current->mm->start_stack;
	(regs)->eflags = 0x200;
	(regs)->cs = __USER32_CS;
	(regs)->ss = __USER32_DS;
	set_fs(USER_DS);
	if (unlikely(current->ptrace & PT_PTRACED)) {
		if (current->ptrace & PT_TRACE_EXEC)
			ptrace_notify ((PTRACE_EVENT_EXEC << 8) | SIGTRAP);
		else
			send_sig(SIGTRAP, current, 0);
	}
	return 0;
}