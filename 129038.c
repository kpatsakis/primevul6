static int load_elf_fdpic_binary(struct linux_binprm *bprm,
				 struct pt_regs *regs)
{
	struct elf_fdpic_params exec_params, interp_params;
	struct elf_phdr *phdr;
	unsigned long stack_size, entryaddr;
#ifndef CONFIG_MMU
	unsigned long fullsize;
#endif
#ifdef ELF_FDPIC_PLAT_INIT
	unsigned long dynaddr;
#endif
	struct file *interpreter = NULL; /* to shut gcc up */
	char *interpreter_name = NULL;
	int executable_stack;
	int retval, i;

	kdebug("____ LOAD %d ____", current->pid);

	memset(&exec_params, 0, sizeof(exec_params));
	memset(&interp_params, 0, sizeof(interp_params));

	exec_params.hdr = *(struct elfhdr *) bprm->buf;
	exec_params.flags = ELF_FDPIC_FLAG_PRESENT | ELF_FDPIC_FLAG_EXECUTABLE;

	/* check that this is a binary we know how to deal with */
	retval = -ENOEXEC;
	if (!is_elf_fdpic(&exec_params.hdr, bprm->file))
		goto error;

	/* read the program header table */
	retval = elf_fdpic_fetch_phdrs(&exec_params, bprm->file);
	if (retval < 0)
		goto error;

	/* scan for a program header that specifies an interpreter */
	phdr = exec_params.phdrs;

	for (i = 0; i < exec_params.hdr.e_phnum; i++, phdr++) {
		switch (phdr->p_type) {
		case PT_INTERP:
			retval = -ENOMEM;
			if (phdr->p_filesz > PATH_MAX)
				goto error;
			retval = -ENOENT;
			if (phdr->p_filesz < 2)
				goto error;

			/* read the name of the interpreter into memory */
			interpreter_name = kmalloc(phdr->p_filesz, GFP_KERNEL);
			if (!interpreter_name)
				goto error;

			retval = kernel_read(bprm->file,
					     phdr->p_offset,
					     interpreter_name,
					     phdr->p_filesz);
			if (retval < 0)
				goto error;

			retval = -ENOENT;
			if (interpreter_name[phdr->p_filesz - 1] != '\0')
				goto error;

			kdebug("Using ELF interpreter %s", interpreter_name);

			/* replace the program with the interpreter */
			interpreter = open_exec(interpreter_name);
			retval = PTR_ERR(interpreter);
			if (IS_ERR(interpreter)) {
				interpreter = NULL;
				goto error;
			}

			/*
			 * If the binary is not readable then enforce
			 * mm->dumpable = 0 regardless of the interpreter's
			 * permissions.
			 */
			if (file_permission(interpreter, MAY_READ) < 0)
				bprm->interp_flags |= BINPRM_FLAGS_ENFORCE_NONDUMP;

			retval = kernel_read(interpreter, 0, bprm->buf,
					     BINPRM_BUF_SIZE);
			if (retval < 0)
				goto error;

			interp_params.hdr = *((struct elfhdr *) bprm->buf);
			break;

		case PT_LOAD:
#ifdef CONFIG_MMU
			if (exec_params.load_addr == 0)
				exec_params.load_addr = phdr->p_vaddr;
#endif
			break;
		}

	}

	if (elf_check_const_displacement(&exec_params.hdr))
		exec_params.flags |= ELF_FDPIC_FLAG_CONSTDISP;

	/* perform insanity checks on the interpreter */
	if (interpreter_name) {
		retval = -ELIBBAD;
		if (!is_elf_fdpic(&interp_params.hdr, interpreter))
			goto error;

		interp_params.flags = ELF_FDPIC_FLAG_PRESENT;

		/* read the interpreter's program header table */
		retval = elf_fdpic_fetch_phdrs(&interp_params, interpreter);
		if (retval < 0)
			goto error;
	}

	stack_size = exec_params.stack_size;
	if (stack_size < interp_params.stack_size)
		stack_size = interp_params.stack_size;

	if (exec_params.flags & ELF_FDPIC_FLAG_EXEC_STACK)
		executable_stack = EXSTACK_ENABLE_X;
	else if (exec_params.flags & ELF_FDPIC_FLAG_NOEXEC_STACK)
		executable_stack = EXSTACK_DISABLE_X;
	else if (interp_params.flags & ELF_FDPIC_FLAG_EXEC_STACK)
		executable_stack = EXSTACK_ENABLE_X;
	else if (interp_params.flags & ELF_FDPIC_FLAG_NOEXEC_STACK)
		executable_stack = EXSTACK_DISABLE_X;
	else
		executable_stack = EXSTACK_DEFAULT;

	retval = -ENOEXEC;
	if (stack_size == 0)
		goto error;

	if (elf_check_const_displacement(&interp_params.hdr))
		interp_params.flags |= ELF_FDPIC_FLAG_CONSTDISP;

	/* flush all traces of the currently running executable */
	retval = flush_old_exec(bprm);
	if (retval)
		goto error;

	/* there's now no turning back... the old userspace image is dead,
	 * defunct, deceased, etc. after this point we have to exit via
	 * error_kill */
	set_personality(PER_LINUX_FDPIC);
	set_binfmt(&elf_fdpic_format);

	current->mm->start_code = 0;
	current->mm->end_code = 0;
	current->mm->start_stack = 0;
	current->mm->start_data = 0;
	current->mm->end_data = 0;
	current->mm->context.exec_fdpic_loadmap = 0;
	current->mm->context.interp_fdpic_loadmap = 0;

	current->flags &= ~PF_FORKNOEXEC;

#ifdef CONFIG_MMU
	elf_fdpic_arch_lay_out_mm(&exec_params,
				  &interp_params,
				  &current->mm->start_stack,
				  &current->mm->start_brk);

	retval = setup_arg_pages(bprm, current->mm->start_stack,
				 executable_stack);
	if (retval < 0) {
		send_sig(SIGKILL, current, 0);
		goto error_kill;
	}
#endif

	/* load the executable and interpreter into memory */
	retval = elf_fdpic_map_file(&exec_params, bprm->file, current->mm,
				    "executable");
	if (retval < 0)
		goto error_kill;

	if (interpreter_name) {
		retval = elf_fdpic_map_file(&interp_params, interpreter,
					    current->mm, "interpreter");
		if (retval < 0) {
			printk(KERN_ERR "Unable to load interpreter\n");
			goto error_kill;
		}

		allow_write_access(interpreter);
		fput(interpreter);
		interpreter = NULL;
	}

#ifdef CONFIG_MMU
	if (!current->mm->start_brk)
		current->mm->start_brk = current->mm->end_data;

	current->mm->brk = current->mm->start_brk =
		PAGE_ALIGN(current->mm->start_brk);

#else
	/* create a stack and brk area big enough for everyone
	 * - the brk heap starts at the bottom and works up
	 * - the stack starts at the top and works down
	 */
	stack_size = (stack_size + PAGE_SIZE - 1) & PAGE_MASK;
	if (stack_size < PAGE_SIZE * 2)
		stack_size = PAGE_SIZE * 2;

	down_write(&current->mm->mmap_sem);
	current->mm->start_brk = do_mmap(NULL, 0, stack_size,
					 PROT_READ | PROT_WRITE | PROT_EXEC,
					 MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN,
					 0);

	if (IS_ERR_VALUE(current->mm->start_brk)) {
		up_write(&current->mm->mmap_sem);
		retval = current->mm->start_brk;
		current->mm->start_brk = 0;
		goto error_kill;
	}

	/* expand the stack mapping to use up the entire allocation granule */
	fullsize = ksize((char *) current->mm->start_brk);
	if (!IS_ERR_VALUE(do_mremap(current->mm->start_brk, stack_size,
				    fullsize, 0, 0)))
		stack_size = fullsize;
	up_write(&current->mm->mmap_sem);

	current->mm->brk = current->mm->start_brk;
	current->mm->context.end_brk = current->mm->start_brk;
	current->mm->context.end_brk +=
		(stack_size > PAGE_SIZE) ? (stack_size - PAGE_SIZE) : 0;
	current->mm->start_stack = current->mm->start_brk + stack_size;
#endif

	compute_creds(bprm);
	current->flags &= ~PF_FORKNOEXEC;
	if (create_elf_fdpic_tables(bprm, current->mm,
				    &exec_params, &interp_params) < 0)
		goto error_kill;

	kdebug("- start_code  %lx", current->mm->start_code);
	kdebug("- end_code    %lx", current->mm->end_code);
	kdebug("- start_data  %lx", current->mm->start_data);
	kdebug("- end_data    %lx", current->mm->end_data);
	kdebug("- start_brk   %lx", current->mm->start_brk);
	kdebug("- brk         %lx", current->mm->brk);
	kdebug("- start_stack %lx", current->mm->start_stack);

#ifdef ELF_FDPIC_PLAT_INIT
	/*
	 * The ABI may specify that certain registers be set up in special
	 * ways (on i386 %edx is the address of a DT_FINI function, for
	 * example.  This macro performs whatever initialization to
	 * the regs structure is required.
	 */
	dynaddr = interp_params.dynamic_addr ?: exec_params.dynamic_addr;
	ELF_FDPIC_PLAT_INIT(regs, exec_params.map_addr, interp_params.map_addr,
			    dynaddr);
#endif

	/* everything is now ready... get the userspace context ready to roll */
	entryaddr = interp_params.entry_addr ?: exec_params.entry_addr;
	start_thread(regs, entryaddr, current->mm->start_stack);

	if (unlikely(current->ptrace & PT_PTRACED)) {
		if (current->ptrace & PT_TRACE_EXEC)
			ptrace_notify((PTRACE_EVENT_EXEC << 8) | SIGTRAP);
		else
			send_sig(SIGTRAP, current, 0);
	}

	retval = 0;

error:
	if (interpreter) {
		allow_write_access(interpreter);
		fput(interpreter);
	}
	kfree(interpreter_name);
	kfree(exec_params.phdrs);
	kfree(exec_params.loadmap);
	kfree(interp_params.phdrs);
	kfree(interp_params.loadmap);
	return retval;

	/* unrecoverable error - kill the process */
error_kill:
	send_sig(SIGSEGV, current, 0);
	goto error;

}