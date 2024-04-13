static int audit_filter_rules(struct task_struct *tsk,
			      struct audit_krule *rule,
			      struct audit_context *ctx,
			      struct audit_names *name,
			      enum audit_state *state)
{
	int i, j, need_sid = 1;
	u32 sid;

	for (i = 0; i < rule->field_count; i++) {
		struct audit_field *f = &rule->fields[i];
		int result = 0;

		switch (f->type) {
		case AUDIT_PID:
			result = audit_comparator(tsk->pid, f->op, f->val);
			break;
		case AUDIT_PPID:
			if (ctx) {
				if (!ctx->ppid)
					ctx->ppid = sys_getppid();
				result = audit_comparator(ctx->ppid, f->op, f->val);
			}
			break;
		case AUDIT_UID:
			result = audit_comparator(tsk->uid, f->op, f->val);
			break;
		case AUDIT_EUID:
			result = audit_comparator(tsk->euid, f->op, f->val);
			break;
		case AUDIT_SUID:
			result = audit_comparator(tsk->suid, f->op, f->val);
			break;
		case AUDIT_FSUID:
			result = audit_comparator(tsk->fsuid, f->op, f->val);
			break;
		case AUDIT_GID:
			result = audit_comparator(tsk->gid, f->op, f->val);
			break;
		case AUDIT_EGID:
			result = audit_comparator(tsk->egid, f->op, f->val);
			break;
		case AUDIT_SGID:
			result = audit_comparator(tsk->sgid, f->op, f->val);
			break;
		case AUDIT_FSGID:
			result = audit_comparator(tsk->fsgid, f->op, f->val);
			break;
		case AUDIT_PERS:
			result = audit_comparator(tsk->personality, f->op, f->val);
			break;
		case AUDIT_ARCH:
 			if (ctx)
				result = audit_comparator(ctx->arch, f->op, f->val);
			break;

		case AUDIT_EXIT:
			if (ctx && ctx->return_valid)
				result = audit_comparator(ctx->return_code, f->op, f->val);
			break;
		case AUDIT_SUCCESS:
			if (ctx && ctx->return_valid) {
				if (f->val)
					result = audit_comparator(ctx->return_valid, f->op, AUDITSC_SUCCESS);
				else
					result = audit_comparator(ctx->return_valid, f->op, AUDITSC_FAILURE);
			}
			break;
		case AUDIT_DEVMAJOR:
			if (name)
				result = audit_comparator(MAJOR(name->dev),
							  f->op, f->val);
			else if (ctx) {
				for (j = 0; j < ctx->name_count; j++) {
					if (audit_comparator(MAJOR(ctx->names[j].dev),	f->op, f->val)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_DEVMINOR:
			if (name)
				result = audit_comparator(MINOR(name->dev),
							  f->op, f->val);
			else if (ctx) {
				for (j = 0; j < ctx->name_count; j++) {
					if (audit_comparator(MINOR(ctx->names[j].dev), f->op, f->val)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_INODE:
			if (name)
				result = (name->ino == f->val);
			else if (ctx) {
				for (j = 0; j < ctx->name_count; j++) {
					if (audit_comparator(ctx->names[j].ino, f->op, f->val)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_WATCH:
			if (name && rule->watch->ino != (unsigned long)-1)
				result = (name->dev == rule->watch->dev &&
					  name->ino == rule->watch->ino);
			break;
		case AUDIT_LOGINUID:
			result = 0;
			if (ctx)
				result = audit_comparator(ctx->loginuid, f->op, f->val);
			break;
		case AUDIT_SUBJ_USER:
		case AUDIT_SUBJ_ROLE:
		case AUDIT_SUBJ_TYPE:
		case AUDIT_SUBJ_SEN:
		case AUDIT_SUBJ_CLR:
			/* NOTE: this may return negative values indicating
			   a temporary error.  We simply treat this as a
			   match for now to avoid losing information that
			   may be wanted.   An error message will also be
			   logged upon error */
			if (f->se_rule) {
				if (need_sid) {
					selinux_get_task_sid(tsk, &sid);
					need_sid = 0;
				}
				result = selinux_audit_rule_match(sid, f->type,
				                                  f->op,
				                                  f->se_rule,
				                                  ctx);
			}
			break;
		case AUDIT_OBJ_USER:
		case AUDIT_OBJ_ROLE:
		case AUDIT_OBJ_TYPE:
		case AUDIT_OBJ_LEV_LOW:
		case AUDIT_OBJ_LEV_HIGH:
			/* The above note for AUDIT_SUBJ_USER...AUDIT_SUBJ_CLR
			   also applies here */
			if (f->se_rule) {
				/* Find files that match */
				if (name) {
					result = selinux_audit_rule_match(
					           name->osid, f->type, f->op,
					           f->se_rule, ctx);
				} else if (ctx) {
					for (j = 0; j < ctx->name_count; j++) {
						if (selinux_audit_rule_match(
						      ctx->names[j].osid,
						      f->type, f->op,
						      f->se_rule, ctx)) {
							++result;
							break;
						}
					}
				}
				/* Find ipc objects that match */
				if (ctx) {
					struct audit_aux_data *aux;
					for (aux = ctx->aux; aux;
					     aux = aux->next) {
						if (aux->type == AUDIT_IPC) {
							struct audit_aux_data_ipcctl *axi = (void *)aux;
							if (selinux_audit_rule_match(axi->osid, f->type, f->op, f->se_rule, ctx)) {
								++result;
								break;
							}
						}
					}
				}
			}
			break;
		case AUDIT_ARG0:
		case AUDIT_ARG1:
		case AUDIT_ARG2:
		case AUDIT_ARG3:
			if (ctx)
				result = audit_comparator(ctx->argv[f->type-AUDIT_ARG0], f->op, f->val);
			break;
		case AUDIT_FILTERKEY:
			/* ignore this field for filtering */
			result = 1;
			break;
		case AUDIT_PERM:
			result = audit_match_perm(ctx, f->val);
			break;
		}

		if (!result)
			return 0;
	}
	if (rule->filterkey)
		ctx->filterkey = kstrdup(rule->filterkey, GFP_ATOMIC);
	switch (rule->action) {
	case AUDIT_NEVER:    *state = AUDIT_DISABLED;	    break;
	case AUDIT_ALWAYS:   *state = AUDIT_RECORD_CONTEXT; break;
	}
	return 1;
}