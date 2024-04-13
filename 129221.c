static int audit_inc_name_count(struct audit_context *context,
				const struct inode *inode)
{
	if (context->name_count >= AUDIT_NAMES) {
		if (inode)
			printk(KERN_DEBUG "name_count maxed, losing inode data: "
			       "dev=%02x:%02x, inode=%lu",
			       MAJOR(inode->i_sb->s_dev),
			       MINOR(inode->i_sb->s_dev),
			       inode->i_ino);

		else
			printk(KERN_DEBUG "name_count maxed, losing inode data");
		return 1;
	}
	context->name_count++;
#if AUDIT_DEBUG
	context->ino_count++;
#endif
	return 0;
}