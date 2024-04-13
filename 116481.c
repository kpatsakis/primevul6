static inline void sanitize_i387_state(struct task_struct *tsk)
{
	if (!use_xsaveopt())
		return;
	__sanitize_i387_state(tsk);
}