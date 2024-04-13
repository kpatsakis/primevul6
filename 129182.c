void audit_core_dumps(long signr)
{
	struct audit_buffer *ab;
	u32 sid;

	if (!audit_enabled)
		return;

	if (signr == SIGQUIT)	/* don't care for those */
		return;

	ab = audit_log_start(NULL, GFP_KERNEL, AUDIT_ANOM_ABEND);
	audit_log_format(ab, "auid=%u uid=%u gid=%u",
			audit_get_loginuid(current->audit_context),
			current->uid, current->gid);
	selinux_get_task_sid(current, &sid);
	if (sid) {
		char *ctx = NULL;
		u32 len;

		if (selinux_sid_to_string(sid, &ctx, &len))
			audit_log_format(ab, " ssid=%u", sid);
		else
			audit_log_format(ab, " subj=%s", ctx);
		kfree(ctx);
	}
	audit_log_format(ab, " pid=%d comm=", current->pid);
	audit_log_untrustedstring(ab, current->comm);
	audit_log_format(ab, " sig=%ld", signr);
	audit_log_end(ab);
}