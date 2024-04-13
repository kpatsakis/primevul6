static void audit_log_exit(struct audit_context *context, struct task_struct *tsk)
{
	int i, call_panic = 0;
	struct audit_buffer *ab;
	struct audit_aux_data *aux;
	const char *tty;

	/* tsk == current */
	context->pid = tsk->pid;
	if (!context->ppid)
		context->ppid = sys_getppid();
	context->uid = tsk->uid;
	context->gid = tsk->gid;
	context->euid = tsk->euid;
	context->suid = tsk->suid;
	context->fsuid = tsk->fsuid;
	context->egid = tsk->egid;
	context->sgid = tsk->sgid;
	context->fsgid = tsk->fsgid;
	context->personality = tsk->personality;

	ab = audit_log_start(context, GFP_KERNEL, AUDIT_SYSCALL);
	if (!ab)
		return;		/* audit_panic has been called */
	audit_log_format(ab, "arch=%x syscall=%d",
			 context->arch, context->major);
	if (context->personality != PER_LINUX)
		audit_log_format(ab, " per=%lx", context->personality);
	if (context->return_valid)
		audit_log_format(ab, " success=%s exit=%ld", 
				 (context->return_valid==AUDITSC_SUCCESS)?"yes":"no",
				 context->return_code);

	mutex_lock(&tty_mutex);
	read_lock(&tasklist_lock);
	if (tsk->signal && tsk->signal->tty && tsk->signal->tty->name)
		tty = tsk->signal->tty->name;
	else
		tty = "(none)";
	read_unlock(&tasklist_lock);
	audit_log_format(ab,
		  " a0=%lx a1=%lx a2=%lx a3=%lx items=%d"
		  " ppid=%d pid=%d auid=%u uid=%u gid=%u"
		  " euid=%u suid=%u fsuid=%u"
		  " egid=%u sgid=%u fsgid=%u tty=%s",
		  context->argv[0],
		  context->argv[1],
		  context->argv[2],
		  context->argv[3],
		  context->name_count,
		  context->ppid,
		  context->pid,
		  context->loginuid,
		  context->uid,
		  context->gid,
		  context->euid, context->suid, context->fsuid,
		  context->egid, context->sgid, context->fsgid, tty);

	mutex_unlock(&tty_mutex);

	audit_log_task_info(ab, tsk);
	if (context->filterkey) {
		audit_log_format(ab, " key=");
		audit_log_untrustedstring(ab, context->filterkey);
	} else
		audit_log_format(ab, " key=(null)");
	audit_log_end(ab);

	for (aux = context->aux; aux; aux = aux->next) {

		ab = audit_log_start(context, GFP_KERNEL, aux->type);
		if (!ab)
			continue; /* audit_panic has been called */

		switch (aux->type) {
		case AUDIT_MQ_OPEN: {
			struct audit_aux_data_mq_open *axi = (void *)aux;
			audit_log_format(ab,
				"oflag=0x%x mode=%#o mq_flags=0x%lx mq_maxmsg=%ld "
				"mq_msgsize=%ld mq_curmsgs=%ld",
				axi->oflag, axi->mode, axi->attr.mq_flags,
				axi->attr.mq_maxmsg, axi->attr.mq_msgsize,
				axi->attr.mq_curmsgs);
			break; }

		case AUDIT_MQ_SENDRECV: {
			struct audit_aux_data_mq_sendrecv *axi = (void *)aux;
			audit_log_format(ab,
				"mqdes=%d msg_len=%zd msg_prio=%u "
				"abs_timeout_sec=%ld abs_timeout_nsec=%ld",
				axi->mqdes, axi->msg_len, axi->msg_prio,
				axi->abs_timeout.tv_sec, axi->abs_timeout.tv_nsec);
			break; }

		case AUDIT_MQ_NOTIFY: {
			struct audit_aux_data_mq_notify *axi = (void *)aux;
			audit_log_format(ab,
				"mqdes=%d sigev_signo=%d",
				axi->mqdes,
				axi->notification.sigev_signo);
			break; }

		case AUDIT_MQ_GETSETATTR: {
			struct audit_aux_data_mq_getsetattr *axi = (void *)aux;
			audit_log_format(ab,
				"mqdes=%d mq_flags=0x%lx mq_maxmsg=%ld mq_msgsize=%ld "
				"mq_curmsgs=%ld ",
				axi->mqdes,
				axi->mqstat.mq_flags, axi->mqstat.mq_maxmsg,
				axi->mqstat.mq_msgsize, axi->mqstat.mq_curmsgs);
			break; }

		case AUDIT_IPC: {
			struct audit_aux_data_ipcctl *axi = (void *)aux;
			audit_log_format(ab, 
				 "ouid=%u ogid=%u mode=%x",
				 axi->uid, axi->gid, axi->mode);
			if (axi->osid != 0) {
				char *ctx = NULL;
				u32 len;
				if (selinux_sid_to_string(
						axi->osid, &ctx, &len)) {
					audit_log_format(ab, " osid=%u",
							axi->osid);
					call_panic = 1;
				} else
					audit_log_format(ab, " obj=%s", ctx);
				kfree(ctx);
			}
			break; }

		case AUDIT_IPC_SET_PERM: {
			struct audit_aux_data_ipcctl *axi = (void *)aux;
			audit_log_format(ab,
				"qbytes=%lx ouid=%u ogid=%u mode=%x",
				axi->qbytes, axi->uid, axi->gid, axi->mode);
			break; }

		case AUDIT_EXECVE: {
			struct audit_aux_data_execve *axi = (void *)aux;
			audit_log_execve_info(ab, axi);
			break; }

		case AUDIT_SOCKETCALL: {
			int i;
			struct audit_aux_data_socketcall *axs = (void *)aux;
			audit_log_format(ab, "nargs=%d", axs->nargs);
			for (i=0; i<axs->nargs; i++)
				audit_log_format(ab, " a%d=%lx", i, axs->args[i]);
			break; }

		case AUDIT_SOCKADDR: {
			struct audit_aux_data_sockaddr *axs = (void *)aux;

			audit_log_format(ab, "saddr=");
			audit_log_hex(ab, axs->a, axs->len);
			break; }

		case AUDIT_AVC_PATH: {
			struct audit_aux_data_path *axi = (void *)aux;
			audit_log_d_path(ab, "path=", axi->dentry, axi->mnt);
			break; }

		case AUDIT_FD_PAIR: {
			struct audit_aux_data_fd_pair *axs = (void *)aux;
			audit_log_format(ab, "fd0=%d fd1=%d", axs->fd[0], axs->fd[1]);
			break; }

		}
		audit_log_end(ab);
	}

	for (aux = context->aux_pids; aux; aux = aux->next) {
		struct audit_aux_data_pids *axs = (void *)aux;
		int i;

		for (i = 0; i < axs->pid_count; i++)
			if (audit_log_pid_context(context, axs->target_pid[i],
						  axs->target_sid[i]))
				call_panic = 1;
	}

	if (context->target_pid &&
	    audit_log_pid_context(context, context->target_pid,
				  context->target_sid))
			call_panic = 1;

	if (context->pwd && context->pwdmnt) {
		ab = audit_log_start(context, GFP_KERNEL, AUDIT_CWD);
		if (ab) {
			audit_log_d_path(ab, "cwd=", context->pwd, context->pwdmnt);
			audit_log_end(ab);
		}
	}
	for (i = 0; i < context->name_count; i++) {
		struct audit_names *n = &context->names[i];

		ab = audit_log_start(context, GFP_KERNEL, AUDIT_PATH);
		if (!ab)
			continue; /* audit_panic has been called */

		audit_log_format(ab, "item=%d", i);

		if (n->name) {
			switch(n->name_len) {
			case AUDIT_NAME_FULL:
				/* log the full path */
				audit_log_format(ab, " name=");
				audit_log_untrustedstring(ab, n->name);
				break;
			case 0:
				/* name was specified as a relative path and the
				 * directory component is the cwd */
				audit_log_d_path(ab, " name=", context->pwd,
						 context->pwdmnt);
				break;
			default:
				/* log the name's directory component */
				audit_log_format(ab, " name=");
				audit_log_n_untrustedstring(ab, n->name_len,
							    n->name);
			}
		} else
			audit_log_format(ab, " name=(null)");

		if (n->ino != (unsigned long)-1) {
			audit_log_format(ab, " inode=%lu"
					 " dev=%02x:%02x mode=%#o"
					 " ouid=%u ogid=%u rdev=%02x:%02x",
					 n->ino,
					 MAJOR(n->dev),
					 MINOR(n->dev),
					 n->mode,
					 n->uid,
					 n->gid,
					 MAJOR(n->rdev),
					 MINOR(n->rdev));
		}
		if (n->osid != 0) {
			char *ctx = NULL;
			u32 len;
			if (selinux_sid_to_string(
				n->osid, &ctx, &len)) {
				audit_log_format(ab, " osid=%u", n->osid);
				call_panic = 2;
			} else
				audit_log_format(ab, " obj=%s", ctx);
			kfree(ctx);
		}

		audit_log_end(ab);
	}
	if (call_panic)
		audit_panic("error converting sid to string");
}