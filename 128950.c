void __audit_inode_child(const char *dname, const struct inode *inode,
			 const struct inode *parent)
{
	int idx;
	struct audit_context *context = current->audit_context;
	const char *found_parent = NULL, *found_child = NULL;
	int dirlen = 0;

	if (!context->in_syscall)
		return;

	/* determine matching parent */
	if (!dname)
		goto add_names;

	/* parent is more likely, look for it first */
	for (idx = 0; idx < context->name_count; idx++) {
		struct audit_names *n = &context->names[idx];

		if (!n->name)
			continue;

		if (n->ino == parent->i_ino &&
		    !audit_compare_dname_path(dname, n->name, &dirlen)) {
			n->name_len = dirlen; /* update parent data in place */
			found_parent = n->name;
			goto add_names;
		}
	}

	/* no matching parent, look for matching child */
	for (idx = 0; idx < context->name_count; idx++) {
		struct audit_names *n = &context->names[idx];

		if (!n->name)
			continue;

		/* strcmp() is the more likely scenario */
		if (!strcmp(dname, n->name) ||
		     !audit_compare_dname_path(dname, n->name, &dirlen)) {
			if (inode)
				audit_copy_inode(n, inode);
			else
				n->ino = (unsigned long)-1;
			found_child = n->name;
			goto add_names;
		}
	}

add_names:
	if (!found_parent) {
		if (audit_inc_name_count(context, parent))
			return;
		idx = context->name_count - 1;
		context->names[idx].name = NULL;
		audit_copy_inode(&context->names[idx], parent);
	}

	if (!found_child) {
		if (audit_inc_name_count(context, inode))
			return;
		idx = context->name_count - 1;

		/* Re-use the name belonging to the slot for a matching parent
		 * directory. All names for this context are relinquished in
		 * audit_free_names() */
		if (found_parent) {
			context->names[idx].name = found_parent;
			context->names[idx].name_len = AUDIT_NAME_FULL;
			/* don't call __putname() */
			context->names[idx].name_put = 0;
		} else {
			context->names[idx].name = NULL;
		}

		if (inode)
			audit_copy_inode(&context->names[idx], inode);
		else
			context->names[idx].ino = (unsigned long)-1;
	}
}