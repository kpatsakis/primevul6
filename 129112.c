static void remove_vma_list(struct mm_struct *mm, struct vm_area_struct *vma)
{
	/* Update high watermark before we lower total_vm */
	update_hiwater_vm(mm);
	do {
		long nrpages = vma_pages(vma);

		mm->total_vm -= nrpages;
		if (vma->vm_flags & VM_LOCKED)
			mm->locked_vm -= nrpages;
		vm_stat_account(mm, vma->vm_flags, vma->vm_file, -nrpages);
		vma = remove_vma(vma);
	} while (vma);
	validate_mm(mm);
}