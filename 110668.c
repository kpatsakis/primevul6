static struct dentry *proc_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	int err;
	struct super_block *sb;
	struct pid_namespace *ns;
	struct proc_inode *ei;
	char *options;

	if (flags & MS_KERNMOUNT) {
		ns = (struct pid_namespace *)data;
		options = NULL;
	} else {
		ns = current->nsproxy->pid_ns;
		options = data;
	}

	sb = sget(fs_type, proc_test_super, proc_set_super, ns);
	if (IS_ERR(sb))
		return ERR_CAST(sb);

	if (!sb->s_root) {
		sb->s_flags = flags;
		if (!proc_parse_options(options, ns)) {
			deactivate_locked_super(sb);
			return ERR_PTR(-EINVAL);
		}
		err = proc_fill_super(sb);
		if (err) {
			deactivate_locked_super(sb);
			return ERR_PTR(err);
		}

		sb->s_flags |= MS_ACTIVE;
	}

	ei = PROC_I(sb->s_root->d_inode);
	if (!ei->pid) {
		rcu_read_lock();
		ei->pid = get_pid(find_pid_ns(1, ns));
		rcu_read_unlock();
	}

	return dget(sb->s_root);
}