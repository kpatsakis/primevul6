static int elf_fdpic_dump_segments(struct file *file, struct mm_struct *mm,
				   size_t *size, unsigned long *limit)
{
	struct vm_list_struct *vml;

	for (vml = current->mm->context.vmlist; vml; vml = vml->next) {
	struct vm_area_struct *vma = vml->vma;

		if (!maydump(vma))
			continue;

		if ((*size += PAGE_SIZE) > *limit)
			return -EFBIG;

		if (!dump_write(file, (void *) vma->vm_start,
				vma->vm_end - vma->vm_start))
			return -EIO;
	}

	return 0;
}