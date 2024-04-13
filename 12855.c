static int selinux_kernfs_init_security(struct kernfs_node *kn_dir,
					struct kernfs_node *kn)
{
	const struct task_security_struct *tsec = selinux_cred(current_cred());
	u32 parent_sid, newsid, clen;
	int rc;
	char *context;

	rc = kernfs_xattr_get(kn_dir, XATTR_NAME_SELINUX, NULL, 0);
	if (rc == -ENODATA)
		return 0;
	else if (rc < 0)
		return rc;

	clen = (u32)rc;
	context = kmalloc(clen, GFP_KERNEL);
	if (!context)
		return -ENOMEM;

	rc = kernfs_xattr_get(kn_dir, XATTR_NAME_SELINUX, context, clen);
	if (rc < 0) {
		kfree(context);
		return rc;
	}

	rc = security_context_to_sid(&selinux_state, context, clen, &parent_sid,
				     GFP_KERNEL);
	kfree(context);
	if (rc)
		return rc;

	if (tsec->create_sid) {
		newsid = tsec->create_sid;
	} else {
		u16 secclass = inode_mode_to_security_class(kn->mode);
		struct qstr q;

		q.name = kn->name;
		q.hash_len = hashlen_string(kn_dir, kn->name);

		rc = security_transition_sid(&selinux_state, tsec->sid,
					     parent_sid, secclass, &q,
					     &newsid);
		if (rc)
			return rc;
	}

	rc = security_sid_to_context_force(&selinux_state, newsid,
					   &context, &clen);
	if (rc)
		return rc;

	rc = kernfs_xattr_set(kn, XATTR_NAME_SELINUX, context, clen,
			      XATTR_CREATE);
	kfree(context);
	return rc;
}