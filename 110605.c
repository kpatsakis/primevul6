
static inline loff_t ext4_isize(struct ext4_inode *raw_inode)
{
	return ((loff_t)le32_to_cpu(raw_inode->i_size_high) << 32) |
		le32_to_cpu(raw_inode->i_size_lo);