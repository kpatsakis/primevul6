static int coredump_wait(int exit_code)
{
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;
	struct completion startup_done;
	struct completion *vfork_done;
	int core_waiters;

	init_completion(&mm->core_done);
	init_completion(&startup_done);
	mm->core_startup_done = &startup_done;

	core_waiters = zap_threads(tsk, mm, exit_code);
	up_write(&mm->mmap_sem);

	if (unlikely(core_waiters < 0))
		goto fail;

	/*
	 * Make sure nobody is waiting for us to release the VM,
	 * otherwise we can deadlock when we wait on each other
	 */
	vfork_done = tsk->vfork_done;
	if (vfork_done) {
		tsk->vfork_done = NULL;
		complete(vfork_done);
	}

	if (core_waiters)
		wait_for_completion(&startup_done);
fail:
	BUG_ON(mm->core_waiters);
	return core_waiters;
}