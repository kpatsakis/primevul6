static inline unsigned short get_fpu_cwd(struct task_struct *tsk)
{
	if (cpu_has_fxsr) {
		return tsk->thread.fpu.state->fxsave.cwd;
	} else {
		return (unsigned short)tsk->thread.fpu.state->fsave.cwd;
	}
}