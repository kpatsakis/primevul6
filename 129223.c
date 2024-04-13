int set_binfmt(struct linux_binfmt *new)
{
	struct linux_binfmt *old = current->binfmt;

	if (new) {
		if (!try_module_get(new->module))
			return -1;
	}
	current->binfmt = new;
	if (old)
		module_put(old->module);
	return 0;
}