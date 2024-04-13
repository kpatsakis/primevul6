void audit_syscall_entry(int arch, int major,
			 unsigned long a1, unsigned long a2,
			 unsigned long a3, unsigned long a4)
{
	struct task_struct *tsk = current;
	struct audit_context *context = tsk->audit_context;
	enum audit_state     state;

	BUG_ON(!context);

	/*
	 * This happens only on certain architectures that make system
	 * calls in kernel_thread via the entry.S interface, instead of
	 * with direct calls.  (If you are porting to a new
	 * architecture, hitting this condition can indicate that you
	 * got the _exit/_leave calls backward in entry.S.)
	 *
	 * i386     no
	 * x86_64   no
	 * ppc64    yes (see arch/powerpc/platforms/iseries/misc.S)
	 *
	 * This also happens with vm86 emulation in a non-nested manner
	 * (entries without exits), so this case must be caught.
	 */
	if (context->in_syscall) {
		struct audit_context *newctx;

#if AUDIT_DEBUG
		printk(KERN_ERR
		       "audit(:%d) pid=%d in syscall=%d;"
		       " entering syscall=%d\n",
		       context->serial, tsk->pid, context->major, major);
#endif
		newctx = audit_alloc_context(context->state);
		if (newctx) {
			newctx->previous   = context;
			context		   = newctx;
			tsk->audit_context = newctx;
		} else	{
			/* If we can't alloc a new context, the best we
			 * can do is to leak memory (any pending putname
			 * will be lost).  The only other alternative is
			 * to abandon auditing. */
			audit_zero_context(context, context->state);
		}
	}
	BUG_ON(context->in_syscall || context->name_count);

	if (!audit_enabled)
		return;

	context->arch	    = arch;
	context->major      = major;
	context->argv[0]    = a1;
	context->argv[1]    = a2;
	context->argv[2]    = a3;
	context->argv[3]    = a4;

	state = context->state;
	context->dummy = !audit_n_rules;
	if (!context->dummy && (state == AUDIT_SETUP_CONTEXT || state == AUDIT_BUILD_CONTEXT))
		state = audit_filter_syscall(tsk, context, &audit_filter_list[AUDIT_FILTER_ENTRY]);
	if (likely(state == AUDIT_DISABLED))
		return;

	context->serial     = 0;
	context->ctime      = CURRENT_TIME;
	context->in_syscall = 1;
	context->auditable  = !!(state == AUDIT_RECORD_CONTEXT);
	context->ppid       = 0;
}