static int __bprm_mm_init(struct linux_binprm *bprm)
{
	int err = -ENOMEM;
	struct vm_area_struct *vma = NULL;
	struct mm_struct *mm = bprm->mm;

	bprm->vma = vma = kmem_cache_zalloc(vm_area_cachep, GFP_KERNEL);
	if (!vma)
		goto err;

	down_write(&mm->mmap_sem);
	vma->vm_mm = mm;

	/*
	 * Place the stack at the largest stack address the architecture
	 * supports. Later, we'll move this to an appropriate place. We don't
	 * use STACK_TOP because that can depend on attributes which aren't
	 * configured yet.
	 */
	vma->vm_end = STACK_TOP_MAX;
	vma->vm_start = vma->vm_end - PAGE_SIZE;

	vma->vm_flags = VM_STACK_FLAGS;
	vma->vm_page_prot = protection_map[vma->vm_flags & 0x7];
	err = insert_vm_struct(mm, vma);
	if (err) {
		up_write(&mm->mmap_sem);
		goto err;
	}

	mm->stack_vm = mm->total_vm = 1;
	up_write(&mm->mmap_sem);

	bprm->p = vma->vm_end - sizeof(void *);

	return 0;

err:
	if (vma) {
		bprm->vma = NULL;
		kmem_cache_free(vm_area_cachep, vma);
	}

	return err;
}