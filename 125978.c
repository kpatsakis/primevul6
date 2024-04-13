static inline struct resv_map *inode_resv_map(struct inode *inode)
{
	/*
	 * At inode evict time, i_mapping may not point to the original
	 * address space within the inode.  This original address space
	 * contains the pointer to the resv_map.  So, always use the
	 * address space embedded within the inode.
	 * The VERY common case is inode->mapping == &inode->i_data but,
	 * this may not be true for device special inodes.
	 */
	return (struct resv_map *)(&inode->i_data)->private_data;
}