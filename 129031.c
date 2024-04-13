static inline void audit_free_names(struct audit_context *context)
{
	int i;

#if AUDIT_DEBUG == 2
	if (context->auditable
	    ||context->put_count + context->ino_count != context->name_count) {
		printk(KERN_ERR "%s:%d(:%d): major=%d in_syscall=%d"
		       " name_count=%d put_count=%d"
		       " ino_count=%d [NOT freeing]\n",
		       __FILE__, __LINE__,
		       context->serial, context->major, context->in_syscall,
		       context->name_count, context->put_count,
		       context->ino_count);
		for (i = 0; i < context->name_count; i++) {
			printk(KERN_ERR "names[%d] = %p = %s\n", i,
			       context->names[i].name,
			       context->names[i].name ?: "(null)");
		}
		dump_stack();
		return;
	}
#endif
#if AUDIT_DEBUG
	context->put_count  = 0;
	context->ino_count  = 0;
#endif

	for (i = 0; i < context->name_count; i++) {
		if (context->names[i].name && context->names[i].name_put)
			__putname(context->names[i].name);
	}
	context->name_count = 0;
	if (context->pwd)
		dput(context->pwd);
	if (context->pwdmnt)
		mntput(context->pwdmnt);
	context->pwd = NULL;
	context->pwdmnt = NULL;
}