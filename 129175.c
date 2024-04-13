int __audit_signal_info(int sig, struct task_struct *t)
{
	struct audit_aux_data_pids *axp;
	struct task_struct *tsk = current;
	struct audit_context *ctx = tsk->audit_context;
	extern pid_t audit_sig_pid;
	extern uid_t audit_sig_uid;
	extern u32 audit_sig_sid;

	if (audit_pid && t->tgid == audit_pid &&
	    (sig == SIGTERM || sig == SIGHUP || sig == SIGUSR1)) {
		audit_sig_pid = tsk->pid;
		if (ctx)
			audit_sig_uid = ctx->loginuid;
		else
			audit_sig_uid = tsk->uid;
		selinux_get_task_sid(tsk, &audit_sig_sid);
	}

	if (!audit_signals) /* audit_context checked in wrapper */
		return 0;

	/* optimize the common case by putting first signal recipient directly
	 * in audit_context */
	if (!ctx->target_pid) {
		ctx->target_pid = t->tgid;
		selinux_get_task_sid(t, &ctx->target_sid);
		return 0;
	}

	axp = (void *)ctx->aux_pids;
	if (!axp || axp->pid_count == AUDIT_AUX_PIDS) {
		axp = kzalloc(sizeof(*axp), GFP_ATOMIC);
		if (!axp)
			return -ENOMEM;

		axp->d.type = AUDIT_OBJ_PID;
		axp->d.next = ctx->aux_pids;
		ctx->aux_pids = (void *)axp;
	}
	BUG_ON(axp->pid_count > AUDIT_AUX_PIDS);

	axp->target_pid[axp->pid_count] = t->tgid;
	selinux_get_task_sid(t, &axp->target_sid[axp->pid_count]);
	axp->pid_count++;

	return 0;
}