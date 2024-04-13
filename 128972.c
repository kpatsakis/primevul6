void audit_log_task_context(struct audit_buffer *ab)
{
	char *ctx = NULL;
	unsigned len;
	int error;
	u32 sid;

	selinux_get_task_sid(current, &sid);
	if (!sid)
		return;

	error = selinux_sid_to_string(sid, &ctx, &len);
	if (error) {
		if (error != -EINVAL)
			goto error_path;
		return;
	}

	audit_log_format(ab, " subj=%s", ctx);
	kfree(ctx);
	return;

error_path:
	audit_panic("error in audit_log_task_context");
	return;
}