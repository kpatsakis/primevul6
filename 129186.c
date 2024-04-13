void __audit_inode(const char *name, const struct inode *inode)
{
	int idx;
	struct audit_context *context = current->audit_context;

	if (!context->in_syscall)
		return;
	if (context->name_count
	    && context->names[context->name_count-1].name
	    && context->names[context->name_count-1].name == name)
		idx = context->name_count - 1;
	else if (context->name_count > 1
		 && context->names[context->name_count-2].name
		 && context->names[context->name_count-2].name == name)
		idx = context->name_count - 2;
	else {
		/* FIXME: how much do we care about inodes that have no
		 * associated name? */
		if (audit_inc_name_count(context, inode))
			return;
		idx = context->name_count - 1;
		context->names[idx].name = NULL;
	}
	audit_copy_inode(&context->names[idx], inode);
}