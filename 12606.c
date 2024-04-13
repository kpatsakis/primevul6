static inline u32 task_sid_binder(const struct task_struct *task)
{
	/*
	 * In many case where this function is used we should be using the
	 * task's subjective SID, but we can't reliably access the subjective
	 * creds of a task other than our own so we must use the objective
	 * creds/SID, which are safe to access.  The downside is that if a task
	 * is temporarily overriding it's creds it will not be reflected here;
	 * however, it isn't clear that binder would handle that case well
	 * anyway.
	 *
	 * If this ever changes and we can safely reference the subjective
	 * creds/SID of another task, this function will make it easier to
	 * identify the various places where we make use of the task SIDs in
	 * the binder code.  It is also likely that we will need to adjust
	 * the main drivers/android binder code as well.
	 */
	return task_sid_obj(task);
}