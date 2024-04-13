unsigned long do_mremap(unsigned long addr,
	unsigned long old_len, unsigned long new_len,
	unsigned long flags, unsigned long new_addr)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long ret = -EINVAL;
	unsigned long charged = 0;

	if (flags & ~(MREMAP_FIXED | MREMAP_MAYMOVE))
		goto out;

	if (addr & ~PAGE_MASK)
		goto out;

	old_len = PAGE_ALIGN(old_len);
	new_len = PAGE_ALIGN(new_len);

	/*
	 * We allow a zero old-len as a special case
	 * for DOS-emu "duplicate shm area" thing. But
	 * a zero new-len is nonsensical.
	 */
	if (!new_len)
		goto out;

	/* new_addr is only valid if MREMAP_FIXED is specified */
	if (flags & MREMAP_FIXED) {
		if (new_addr & ~PAGE_MASK)
			goto out;
		if (!(flags & MREMAP_MAYMOVE))
			goto out;

		if (new_len > TASK_SIZE || new_addr > TASK_SIZE - new_len)
			goto out;

		/* Check if the location we're moving into overlaps the
		 * old location at all, and fail if it does.
		 */
		if ((new_addr <= addr) && (new_addr+new_len) > addr)
			goto out;

		if ((addr <= new_addr) && (addr+old_len) > new_addr)
			goto out;

		ret = security_file_mmap(0, 0, 0, 0, new_addr, 1);
		if (ret)
			goto out;

		ret = do_munmap(mm, new_addr, new_len);
		if (ret)
			goto out;
	}

	/*
	 * Always allow a shrinking remap: that just unmaps
	 * the unnecessary pages..
	 * do_munmap does all the needed commit accounting
	 */
	if (old_len >= new_len) {
		ret = do_munmap(mm, addr+new_len, old_len - new_len);
		if (ret && old_len != new_len)
			goto out;
		ret = addr;
		if (!(flags & MREMAP_FIXED) || (new_addr == addr))
			goto out;
		old_len = new_len;
	}

	/*
	 * Ok, we need to grow..  or relocate.
	 */
	ret = -EFAULT;
	vma = find_vma(mm, addr);
	if (!vma || vma->vm_start > addr)
		goto out;
	if (is_vm_hugetlb_page(vma)) {
		ret = -EINVAL;
		goto out;
	}
	/* We can't remap across vm area boundaries */
	if (old_len > vma->vm_end - addr)
		goto out;
	if (vma->vm_flags & (VM_DONTEXPAND | VM_PFNMAP)) {
		if (new_len > old_len)
			goto out;
	}
	if (vma->vm_flags & VM_LOCKED) {
		unsigned long locked, lock_limit;
		locked = mm->locked_vm << PAGE_SHIFT;
		lock_limit = current->signal->rlim[RLIMIT_MEMLOCK].rlim_cur;
		locked += new_len - old_len;
		ret = -EAGAIN;
		if (locked > lock_limit && !capable(CAP_IPC_LOCK))
			goto out;
	}
	if (!may_expand_vm(mm, (new_len - old_len) >> PAGE_SHIFT)) {
		ret = -ENOMEM;
		goto out;
	}

	if (vma->vm_flags & VM_ACCOUNT) {
		charged = (new_len - old_len) >> PAGE_SHIFT;
		if (security_vm_enough_memory(charged))
			goto out_nc;
	}

	/* old_len exactly to the end of the area..
	 * And we're not relocating the area.
	 */
	if (old_len == vma->vm_end - addr &&
	    !((flags & MREMAP_FIXED) && (addr != new_addr)) &&
	    (old_len != new_len || !(flags & MREMAP_MAYMOVE))) {
		unsigned long max_addr = TASK_SIZE;
		if (vma->vm_next)
			max_addr = vma->vm_next->vm_start;
		/* can we just expand the current mapping? */
		if (max_addr - addr >= new_len) {
			int pages = (new_len - old_len) >> PAGE_SHIFT;

			vma_adjust(vma, vma->vm_start,
				addr + new_len, vma->vm_pgoff, NULL);

			mm->total_vm += pages;
			vm_stat_account(mm, vma->vm_flags, vma->vm_file, pages);
			if (vma->vm_flags & VM_LOCKED) {
				mm->locked_vm += pages;
				make_pages_present(addr + old_len,
						   addr + new_len);
			}
			ret = addr;
			goto out;
		}
	}

	/*
	 * We weren't able to just expand or shrink the area,
	 * we need to create a new one and move it..
	 */
	ret = -ENOMEM;
	if (flags & MREMAP_MAYMOVE) {
		if (!(flags & MREMAP_FIXED)) {
			unsigned long map_flags = 0;
			if (vma->vm_flags & VM_MAYSHARE)
				map_flags |= MAP_SHARED;

			new_addr = get_unmapped_area(vma->vm_file, 0, new_len,
						vma->vm_pgoff, map_flags);
			if (new_addr & ~PAGE_MASK) {
				ret = new_addr;
				goto out;
			}

			ret = security_file_mmap(0, 0, 0, 0, new_addr, 1);
			if (ret)
				goto out;
		}
		ret = move_vma(vma, addr, old_len, new_len, new_addr);
	}
out:
	if (ret & ~PAGE_MASK)
		vm_unacct_memory(charged);
out_nc:
	return ret;
}