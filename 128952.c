void audit_putname(const char *name)
{
	struct audit_context *context = current->audit_context;

	BUG_ON(!context);
	if (!context->in_syscall) {
#if AUDIT_DEBUG == 2
		printk(KERN_ERR "%s:%d(:%d): __putname(%p)\n",
		       __FILE__, __LINE__, context->serial, name);
		if (context->name_count) {
			int i;
			for (i = 0; i < context->name_count; i++)
				printk(KERN_ERR "name[%d] = %p = %s\n", i,
				       context->names[i].name,
				       context->names[i].name ?: "(null)");
		}
#endif
		__putname(name);
	}
#if AUDIT_DEBUG
	else {
		++context->put_count;
		if (context->put_count > context->name_count) {
			printk(KERN_ERR "%s:%d(:%d): major=%d"
			       " in_syscall=%d putname(%p) name_count=%d"
			       " put_count=%d\n",
			       __FILE__, __LINE__,
			       context->serial, context->major,
			       context->in_syscall, name, context->name_count,
			       context->put_count);
			dump_stack();
		}
	}
#endif
}