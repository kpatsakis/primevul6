static int selinux_inode_init_security_anon(struct inode *inode,
					    const struct qstr *name,
					    const struct inode *context_inode)
{
	const struct task_security_struct *tsec = selinux_cred(current_cred());
	struct common_audit_data ad;
	struct inode_security_struct *isec;
	int rc;

	if (unlikely(!selinux_initialized(&selinux_state)))
		return 0;

	isec = selinux_inode(inode);

	/*
	 * We only get here once per ephemeral inode.  The inode has
	 * been initialized via inode_alloc_security but is otherwise
	 * untouched.
	 */

	if (context_inode) {
		struct inode_security_struct *context_isec =
			selinux_inode(context_inode);
		if (context_isec->initialized != LABEL_INITIALIZED) {
			pr_err("SELinux:  context_inode is not initialized");
			return -EACCES;
		}

		isec->sclass = context_isec->sclass;
		isec->sid = context_isec->sid;
	} else {
		isec->sclass = SECCLASS_ANON_INODE;
		rc = security_transition_sid(
			&selinux_state, tsec->sid, tsec->sid,
			isec->sclass, name, &isec->sid);
		if (rc)
			return rc;
	}

	isec->initialized = LABEL_INITIALIZED;
	/*
	 * Now that we've initialized security, check whether we're
	 * allowed to actually create this type of anonymous inode.
	 */

	ad.type = LSM_AUDIT_DATA_INODE;
	ad.u.inode = inode;

	return avc_has_perm(&selinux_state,
			    tsec->sid,
			    isec->sid,
			    isec->sclass,
			    FILE__CREATE,
			    &ad);
}