struct vm_area_struct *copy_vma(struct vm_area_struct **vmap,
	unsigned long addr, unsigned long len, pgoff_t pgoff)
{
	struct vm_area_struct *vma = *vmap;
	unsigned long vma_start = vma->vm_start;
	struct mm_struct *mm = vma->vm_mm;
	struct vm_area_struct *new_vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	struct mempolicy *pol;

	/*
	 * If anonymous vma has not yet been faulted, update new pgoff
	 * to match new location, to increase its chance of merging.
	 */
	if (!vma->vm_file && !vma->anon_vma)
		pgoff = addr >> PAGE_SHIFT;

	find_vma_prepare(mm, addr, &prev, &rb_link, &rb_parent);
	new_vma = vma_merge(mm, prev, addr, addr + len, vma->vm_flags,
			vma->anon_vma, vma->vm_file, pgoff, vma_policy(vma));
	if (new_vma) {
		/*
		 * Source vma may have been merged into new_vma
		 */
		if (vma_start >= new_vma->vm_start &&
		    vma_start < new_vma->vm_end)
			*vmap = new_vma;
	} else {
		new_vma = kmem_cache_alloc(vm_area_cachep, GFP_KERNEL);
		if (new_vma) {
			*new_vma = *vma;
			pol = mpol_copy(vma_policy(vma));
			if (IS_ERR(pol)) {
				kmem_cache_free(vm_area_cachep, new_vma);
				return NULL;
			}
			vma_set_policy(new_vma, pol);
			new_vma->vm_start = addr;
			new_vma->vm_end = addr + len;
			new_vma->vm_pgoff = pgoff;
			if (new_vma->vm_file)
				get_file(new_vma->vm_file);
			if (new_vma->vm_ops && new_vma->vm_ops->open)
				new_vma->vm_ops->open(new_vma);
			vma_link(mm, new_vma, prev, rb_link, rb_parent);
		}
	}
	return new_vma;
}