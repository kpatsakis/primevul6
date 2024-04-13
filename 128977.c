void set_task_comm(struct task_struct *tsk, char *buf)
{
	task_lock(tsk);
	strlcpy(tsk->comm, buf, sizeof(tsk->comm));
	task_unlock(tsk);
}