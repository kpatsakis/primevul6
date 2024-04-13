void __audit_getname(const char *name)
{
	struct audit_context *context = current->audit_context;

	if (IS_ERR(name) || !name)
		return;

	if (!context->in_syscall) {
#if AUDIT_DEBUG == 2
		printk(KERN_ERR "%s:%d(:%d): ignoring getname(%p)\n",
		       __FILE__, __LINE__, context->serial, name);
		dump_stack();
#endif
		return;
	}
	BUG_ON(context->name_count >= AUDIT_NAMES);
	context->names[context->name_count].name = name;
	context->names[context->name_count].name_len = AUDIT_NAME_FULL;
	context->names[context->name_count].name_put = 1;
	context->names[context->name_count].ino  = (unsigned long)-1;
	context->names[context->name_count].osid = 0;
	++context->name_count;
	if (!context->pwd) {
		read_lock(&current->fs->lock);
		context->pwd = dget(current->fs->pwd);
		context->pwdmnt = mntget(current->fs->pwdmnt);
		read_unlock(&current->fs->lock);
	}
		
}