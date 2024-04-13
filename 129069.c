struct anon_vma *find_mergeable_anon_vma(struct vm_area_struct *vma)
{
	struct vm_area_struct *near;
	unsigned long vm_flags;

	near = vma->vm_next;
	if (!near)
		goto try_prev;

	/*
	 * Since only mprotect tries to remerge vmas, match flags
	 * which might be mprotected into each other later on.
	 * Neither mlock nor madvise tries to remerge at present,
	 * so leave their flags as obstructing a merge.
	 */
	vm_flags = vma->vm_flags & ~(VM_READ|VM_WRITE|VM_EXEC);
	vm_flags |= near->vm_flags & (VM_READ|VM_WRITE|VM_EXEC);

	if (near->anon_vma && vma->vm_end == near->vm_start &&
 			mpol_equal(vma_policy(vma), vma_policy(near)) &&
			can_vma_merge_before(near, vm_flags,
				NULL, vma->vm_file, vma->vm_pgoff +
				((vma->vm_end - vma->vm_start) >> PAGE_SHIFT)))
		return near->anon_vma;
try_prev:
	/*
	 * It is potentially slow to have to call find_vma_prev here.
	 * But it's only on the first write fault on the vma, not
	 * every time, and we could devise a way to avoid it later
	 * (e.g. stash info in next's anon_vma_node when assigning
	 * an anon_vma, or when trying vma_merge).  Another time.
	 */
	BUG_ON(find_vma_prev(vma->vm_mm, vma->vm_start, &near) != vma);
	if (!near)
		goto none;

	vm_flags = vma->vm_flags & ~(VM_READ|VM_WRITE|VM_EXEC);
	vm_flags |= near->vm_flags & (VM_READ|VM_WRITE|VM_EXEC);

	if (near->anon_vma && near->vm_end == vma->vm_start &&
  			mpol_equal(vma_policy(near), vma_policy(vma)) &&
			can_vma_merge_after(near, vm_flags,
				NULL, vma->vm_file, vma->vm_pgoff))
		return near->anon_vma;
none:
	/*
	 * There's no absolute need to look only at touching neighbours:
	 * we could search further afield for "compatible" anon_vmas.
	 * But it would probably just be a waste of time searching,
	 * or lead to too many vmas hanging off the same anon_vma.
	 * We're trying to allow mprotect remerging later on,
	 * not trying to minimize memory used for anon_vmas.
	 */
	return NULL;
}