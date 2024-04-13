static int audit_log_pid_context(struct audit_context *context, pid_t pid,
				 u32 sid)
{
	struct audit_buffer *ab;
	char *s = NULL;
	u32 len;
	int rc = 0;

	ab = audit_log_start(context, GFP_KERNEL, AUDIT_OBJ_PID);
	if (!ab)
		return 1;

	if (selinux_sid_to_string(sid, &s, &len)) {
		audit_log_format(ab, "opid=%d obj=(none)", pid);
		rc = 1;
	} else
		audit_log_format(ab, "opid=%d  obj=%s", pid, s);
	audit_log_end(ab);
	kfree(s);

	return rc;
}