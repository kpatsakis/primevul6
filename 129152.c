static void audit_log_execve_info(struct audit_buffer *ab,
		struct audit_aux_data_execve *axi)
{
	int i;
	long len, ret;
	const char __user *p = (const char __user *)axi->mm->arg_start;
	char *buf;

	if (axi->mm != current->mm)
		return; /* execve failed, no additional info */

	for (i = 0; i < axi->argc; i++, p += len) {
		len = strnlen_user(p, MAX_ARG_STRLEN);
		/*
		 * We just created this mm, if we can't find the strings
		 * we just copied into it something is _very_ wrong. Similar
		 * for strings that are too long, we should not have created
		 * any.
		 */
		if (!len || len > MAX_ARG_STRLEN) {
			WARN_ON(1);
			send_sig(SIGKILL, current, 0);
		}

		buf = kmalloc(len, GFP_KERNEL);
		if (!buf) {
			audit_panic("out of memory for argv string\n");
			break;
		}

		ret = copy_from_user(buf, p, len);
		/*
		 * There is no reason for this copy to be short. We just
		 * copied them here, and the mm hasn't been exposed to user-
		 * space yet.
		 */
		if (!ret) {
			WARN_ON(1);
			send_sig(SIGKILL, current, 0);
		}

		audit_log_format(ab, "a%d=", i);
		audit_log_untrustedstring(ab, buf);
		audit_log_format(ab, "\n");

		kfree(buf);
	}
}