static inline void __save_fpu(struct task_struct *tsk)
{
	if (use_xsave())
		xsave_state(&tsk->thread.fpu.state->xsave, -1);
	else
		fpu_fxsave(&tsk->thread.fpu);
}