void exit_thread(struct task_struct *tsk)
{
	struct thread_struct *t = &tsk->thread;
	struct fpu *fpu = &t->fpu;

	if (test_thread_flag(TIF_IO_BITMAP))
		io_bitmap_exit(tsk);

	free_vm86(t);

	fpu__drop(fpu);
}