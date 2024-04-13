static void kill_node(Node *e)
{
	struct dentry *dentry;

	write_lock(&entries_lock);
	dentry = e->dentry;
	if (dentry) {
		list_del_init(&e->list);
		e->dentry = NULL;
	}
	write_unlock(&entries_lock);

	if (dentry) {
		dentry->d_inode->i_nlink--;
		d_drop(dentry);
		dput(dentry);
		simple_release_fs(&bm_mnt, &entry_count);
	}
}