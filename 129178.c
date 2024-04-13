static int maydump(struct vm_area_struct *vma)
{
	/* Do not dump I/O mapped devices or special mappings */
	if (vma->vm_flags & (VM_IO | VM_RESERVED)) {
		kdcore("%08lx: %08lx: no (IO)", vma->vm_start, vma->vm_flags);
		return 0;
	}

	/* If we may not read the contents, don't allow us to dump
	 * them either. "dump_write()" can't handle it anyway.
	 */
	if (!(vma->vm_flags & VM_READ)) {
		kdcore("%08lx: %08lx: no (!read)", vma->vm_start, vma->vm_flags);
		return 0;
	}

	/* Dump shared memory only if mapped from an anonymous file. */
	if (vma->vm_flags & VM_SHARED) {
		if (vma->vm_file->f_path.dentry->d_inode->i_nlink == 0) {
			kdcore("%08lx: %08lx: no (share)", vma->vm_start, vma->vm_flags);
			return 1;
		}

		kdcore("%08lx: %08lx: no (share)", vma->vm_start, vma->vm_flags);
		return 0;
	}

#ifdef CONFIG_MMU
	/* If it hasn't been written to, don't write it out */
	if (!vma->anon_vma) {
		kdcore("%08lx: %08lx: no (!anon)", vma->vm_start, vma->vm_flags);
		return 0;
	}
#endif

	kdcore("%08lx: %08lx: yes", vma->vm_start, vma->vm_flags);
	return 1;
}