static int create_elf_fdpic_tables(struct linux_binprm *bprm,
				   struct mm_struct *mm,
				   struct elf_fdpic_params *exec_params,
				   struct elf_fdpic_params *interp_params)
{
	unsigned long sp, csp, nitems;
	elf_caddr_t __user *argv, *envp;
	size_t platform_len = 0, len;
	char *k_platform;
	char __user *u_platform, *p;
	long hwcap;
	int loop;

	/* we're going to shovel a whole load of stuff onto the stack */
#ifdef CONFIG_MMU
	sp = bprm->p;
#else
	sp = mm->start_stack;

	/* stack the program arguments and environment */
	if (elf_fdpic_transfer_args_to_stack(bprm, &sp) < 0)
		return -EFAULT;
#endif

	/* get hold of platform and hardware capabilities masks for the machine
	 * we are running on.  In some cases (Sparc), this info is impossible
	 * to get, in others (i386) it is merely difficult.
	 */
	hwcap = ELF_HWCAP;
	k_platform = ELF_PLATFORM;
	u_platform = NULL;

	if (k_platform) {
		platform_len = strlen(k_platform) + 1;
		sp -= platform_len;
		u_platform = (char __user *) sp;
		if (__copy_to_user(u_platform, k_platform, platform_len) != 0)
			return -EFAULT;
	}

#if defined(__i386__) && defined(CONFIG_SMP)
	/* in some cases (e.g. Hyper-Threading), we want to avoid L1 evictions
	 * by the processes running on the same package. One thing we can do is
	 * to shuffle the initial stack for them.
	 *
	 * the conditionals here are unneeded, but kept in to make the code
	 * behaviour the same as pre change unless we have hyperthreaded
	 * processors. This keeps Mr Marcelo Person happier but should be
	 * removed for 2.5
	 */
	if (smp_num_siblings > 1)
		sp = sp - ((current->pid % 64) << 7);
#endif

	sp &= ~7UL;

	/* stack the load map(s) */
	len = sizeof(struct elf32_fdpic_loadmap);
	len += sizeof(struct elf32_fdpic_loadseg) * exec_params->loadmap->nsegs;
	sp = (sp - len) & ~7UL;
	exec_params->map_addr = sp;

	if (copy_to_user((void __user *) sp, exec_params->loadmap, len) != 0)
		return -EFAULT;

	current->mm->context.exec_fdpic_loadmap = (unsigned long) sp;

	if (interp_params->loadmap) {
		len = sizeof(struct elf32_fdpic_loadmap);
		len += sizeof(struct elf32_fdpic_loadseg) *
			interp_params->loadmap->nsegs;
		sp = (sp - len) & ~7UL;
		interp_params->map_addr = sp;

		if (copy_to_user((void __user *) sp, interp_params->loadmap,
				 len) != 0)
			return -EFAULT;

		current->mm->context.interp_fdpic_loadmap = (unsigned long) sp;
	}

	/* force 16 byte _final_ alignment here for generality */
#define DLINFO_ITEMS 13

	nitems = 1 + DLINFO_ITEMS + (k_platform ? 1 : 0);
#ifdef DLINFO_ARCH_ITEMS
	nitems += DLINFO_ARCH_ITEMS;
#endif

	csp = sp;
	sp -= nitems * 2 * sizeof(unsigned long);
	sp -= (bprm->envc + 1) * sizeof(char *);	/* envv[] */
	sp -= (bprm->argc + 1) * sizeof(char *);	/* argv[] */
	sp -= 1 * sizeof(unsigned long);		/* argc */

	csp -= sp & 15UL;
	sp -= sp & 15UL;

	/* put the ELF interpreter info on the stack */
#define NEW_AUX_ENT(nr, id, val)					\
	do {								\
		struct { unsigned long _id, _val; } __user *ent;	\
									\
		ent = (void __user *) csp;				\
		__put_user((id), &ent[nr]._id);				\
		__put_user((val), &ent[nr]._val);			\
	} while (0)

	csp -= 2 * sizeof(unsigned long);
	NEW_AUX_ENT(0, AT_NULL, 0);
	if (k_platform) {
		csp -= 2 * sizeof(unsigned long);
		NEW_AUX_ENT(0, AT_PLATFORM,
			    (elf_addr_t) (unsigned long) u_platform);
	}

	csp -= DLINFO_ITEMS * 2 * sizeof(unsigned long);
	NEW_AUX_ENT( 0, AT_HWCAP,	hwcap);
	NEW_AUX_ENT( 1, AT_PAGESZ,	PAGE_SIZE);
	NEW_AUX_ENT( 2, AT_CLKTCK,	CLOCKS_PER_SEC);
	NEW_AUX_ENT( 3, AT_PHDR,	exec_params->ph_addr);
	NEW_AUX_ENT( 4, AT_PHENT,	sizeof(struct elf_phdr));
	NEW_AUX_ENT( 5, AT_PHNUM,	exec_params->hdr.e_phnum);
	NEW_AUX_ENT( 6,	AT_BASE,	interp_params->elfhdr_addr);
	NEW_AUX_ENT( 7, AT_FLAGS,	0);
	NEW_AUX_ENT( 8, AT_ENTRY,	exec_params->entry_addr);
	NEW_AUX_ENT( 9, AT_UID,		(elf_addr_t) current->uid);
	NEW_AUX_ENT(10, AT_EUID,	(elf_addr_t) current->euid);
	NEW_AUX_ENT(11, AT_GID,		(elf_addr_t) current->gid);
	NEW_AUX_ENT(12, AT_EGID,	(elf_addr_t) current->egid);

#ifdef ARCH_DLINFO
	/* ARCH_DLINFO must come last so platform specific code can enforce
	 * special alignment requirements on the AUXV if necessary (eg. PPC).
	 */
	ARCH_DLINFO;
#endif
#undef NEW_AUX_ENT

	/* allocate room for argv[] and envv[] */
	csp -= (bprm->envc + 1) * sizeof(elf_caddr_t);
	envp = (elf_caddr_t __user *) csp;
	csp -= (bprm->argc + 1) * sizeof(elf_caddr_t);
	argv = (elf_caddr_t __user *) csp;

	/* stack argc */
	csp -= sizeof(unsigned long);
	__put_user(bprm->argc, (unsigned long __user *) csp);

	BUG_ON(csp != sp);

	/* fill in the argv[] array */
#ifdef CONFIG_MMU
	current->mm->arg_start = bprm->p;
#else
	current->mm->arg_start = current->mm->start_stack -
		(MAX_ARG_PAGES * PAGE_SIZE - bprm->p);
#endif

	p = (char __user *) current->mm->arg_start;
	for (loop = bprm->argc; loop > 0; loop--) {
		__put_user((elf_caddr_t) p, argv++);
		len = strnlen_user(p, MAX_ARG_STRLEN);
		if (!len || len > MAX_ARG_STRLEN)
			return -EINVAL;
		p += len;
	}
	__put_user(NULL, argv);
	current->mm->arg_end = (unsigned long) p;

	/* fill in the envv[] array */
	current->mm->env_start = (unsigned long) p;
	for (loop = bprm->envc; loop > 0; loop--) {
		__put_user((elf_caddr_t)(unsigned long) p, envp++);
		len = strnlen_user(p, MAX_ARG_STRLEN);
		if (!len || len > MAX_ARG_STRLEN)
			return -EINVAL;
		p += len;
	}
	__put_user(NULL, envp);
	current->mm->env_end = (unsigned long) p;

	mm->start_stack = (unsigned long) sp;
	return 0;
}