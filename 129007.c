static int elf_fdpic_dump_segments(struct file *file, struct mm_struct *mm,
				   size_t *size, unsigned long *limit)
{
	struct vm_area_struct *vma;

	for (vma = current->mm->mmap; vma; vma = vma->vm_next) {
		unsigned long addr;

		if (!maydump(vma))
			continue;

		for (addr = vma->vm_start;
		     addr < vma->vm_end;
		     addr += PAGE_SIZE
		     ) {
			struct vm_area_struct *vma;
			struct page *page;

			if (get_user_pages(current, current->mm, addr, 1, 0, 1,
					   &page, &vma) <= 0) {
				DUMP_SEEK(file->f_pos + PAGE_SIZE);
			}
			else if (page == ZERO_PAGE(addr)) {
				page_cache_release(page);
				DUMP_SEEK(file->f_pos + PAGE_SIZE);
			}
			else {
				void *kaddr;

				flush_cache_page(vma, addr, page_to_pfn(page));
				kaddr = kmap(page);
				if ((*size += PAGE_SIZE) > *limit ||
				    !dump_write(file, kaddr, PAGE_SIZE)
				    ) {
					kunmap(page);
					page_cache_release(page);
					return -EIO;
				}
				kunmap(page);
				page_cache_release(page);
			}
		}
	}

	return 0;

end_coredump:
	return -EFBIG;
}