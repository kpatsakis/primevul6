static ssize_t bm_status_write(struct file * file, const char __user * buffer,
		size_t count, loff_t *ppos)
{
	int res = parse_command(buffer, count);
	struct dentry *root;

	switch (res) {
		case 1: enabled = 0; break;
		case 2: enabled = 1; break;
		case 3: root = dget(file->f_path.mnt->mnt_sb->s_root);
			mutex_lock(&root->d_inode->i_mutex);

			while (!list_empty(&entries))
				kill_node(list_entry(entries.next, Node, list));

			mutex_unlock(&root->d_inode->i_mutex);
			dput(root);
		default: return res;
	}
	return count;
}