static u32 __user *create_aout_tables(char __user *p, struct linux_binprm *bprm)
{
	u32 __user *argv;
	u32 __user *envp;
	u32 __user *sp;
	int argc = bprm->argc;
	int envc = bprm->envc;

	sp = (u32 __user *) ((-(unsigned long)sizeof(u32)) & (unsigned long) p);
	sp -= envc+1;
	envp = sp;
	sp -= argc+1;
	argv = sp;
	put_user((unsigned long) envp,--sp);
	put_user((unsigned long) argv,--sp);
	put_user(argc,--sp);
	current->mm->arg_start = (unsigned long) p;
	while (argc-->0) {
		char c;
		put_user((u32)(unsigned long)p,argv++);
		do {
			get_user(c,p++);
		} while (c);
	}
	put_user(0, argv);
	current->mm->arg_end = current->mm->env_start = (unsigned long) p;
	while (envc-->0) {
		char c;
		put_user((u32)(unsigned long)p,envp++);
		do {
			get_user(c,p++);
		} while (c);
	}
	put_user(0, envp);
	current->mm->env_end = (unsigned long) p;
	return sp;
}