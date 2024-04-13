int split_vma(struct mm_struct * mm, struct vm_area_struct * vma,
	      unsigned long addr, int new_below)
{
	struct mempolicy *pol;
	struct vm_area_struct *new;

	if (is_vm_hugetlb_page(vma) && (addr & ~HPAGE_MASK))
		return -EINVAL;

	if (mm->map_count >= sysctl_max_map_count)
		return -ENOMEM;

	new = kmem_cache_alloc(vm_area_cachep, GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	/* most fields are the same, copy all, and then fixup */
	*new = *vma;

	if (new_below)
		new->vm_end = addr;
	else {
		new->vm_start = addr;
		new->vm_pgoff += ((addr - vma->vm_start) >> PAGE_SHIFT);
	}

	pol = mpol_copy(vma_policy(vma));
	if (IS_ERR(pol)) {
		kmem_cache_free(vm_area_cachep, new);
		return PTR_ERR(pol);
	}
	vma_set_policy(new, pol);

	if (new->vm_file)
		get_file(new->vm_file);

	if (new->vm_ops && new->vm_ops->open)
		new->vm_ops->open(new);

	if (new_below)
		vma_adjust(vma, addr, vma->vm_end, vma->vm_pgoff +
			((addr - new->vm_start) >> PAGE_SHIFT), new);
	else
		vma_adjust(vma, vma->vm_start, addr, vma->vm_pgoff, new);

	return 0;
}