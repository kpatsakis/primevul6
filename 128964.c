static int maydump(struct vm_area_struct *vma)
{
	/* The vma can be set up to tell us the answer directly.  */
	if (vma->vm_flags & VM_ALWAYSDUMP)
		return 1;

	/* Do not dump I/O mapped devices or special mappings */
	if (vma->vm_flags & (VM_IO | VM_RESERVED))
		return 0;

	/* Dump shared memory only if mapped from an anonymous file. */
	if (vma->vm_flags & VM_SHARED)
		return vma->vm_file->f_path.dentry->d_inode->i_nlink == 0;

	/* If it hasn't been written to, don't write it out */
	if (!vma->anon_vma)
		return 0;

	return 1;
}