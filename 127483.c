int copy_thread_tls(unsigned long clone_flags, unsigned long sp,
		    unsigned long arg, struct task_struct *p, unsigned long tls)
{
	struct inactive_task_frame *frame;
	struct fork_frame *fork_frame;
	struct pt_regs *childregs;
	int ret = 0;

	childregs = task_pt_regs(p);
	fork_frame = container_of(childregs, struct fork_frame, regs);
	frame = &fork_frame->frame;

	frame->bp = 0;
	frame->ret_addr = (unsigned long) ret_from_fork;
	p->thread.sp = (unsigned long) fork_frame;
	p->thread.io_bitmap = NULL;
	memset(p->thread.ptrace_bps, 0, sizeof(p->thread.ptrace_bps));

#ifdef CONFIG_X86_64
	savesegment(gs, p->thread.gsindex);
	p->thread.gsbase = p->thread.gsindex ? 0 : current->thread.gsbase;
	savesegment(fs, p->thread.fsindex);
	p->thread.fsbase = p->thread.fsindex ? 0 : current->thread.fsbase;
	savesegment(es, p->thread.es);
	savesegment(ds, p->thread.ds);
#else
	p->thread.sp0 = (unsigned long) (childregs + 1);
	/*
	 * Clear all status flags including IF and set fixed bit. 64bit
	 * does not have this initialization as the frame does not contain
	 * flags. The flags consistency (especially vs. AC) is there
	 * ensured via objtool, which lacks 32bit support.
	 */
	frame->flags = X86_EFLAGS_FIXED;
#endif

	/* Kernel thread ? */
	if (unlikely(p->flags & PF_KTHREAD)) {
		memset(childregs, 0, sizeof(struct pt_regs));
		kthread_frame_init(frame, sp, arg);
		return 0;
	}

	frame->bx = 0;
	*childregs = *current_pt_regs();
	childregs->ax = 0;
	if (sp)
		childregs->sp = sp;

#ifdef CONFIG_X86_32
	task_user_gs(p) = get_user_gs(current_pt_regs());
#endif

	/* Set a new TLS for the child thread? */
	if (clone_flags & CLONE_SETTLS)
		ret = set_new_tls(p, tls);

	if (!ret && unlikely(test_tsk_thread_flag(current, TIF_IO_BITMAP)))
		io_bitmap_share(p);

	return ret;
}