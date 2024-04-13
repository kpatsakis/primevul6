unsigned long move_page_tables(struct vm_area_struct *vma,
		unsigned long old_addr, struct vm_area_struct *new_vma,
		unsigned long new_addr, unsigned long len)
{
	unsigned long extent, next, old_end;
	pmd_t *old_pmd, *new_pmd;

	old_end = old_addr + len;
	flush_cache_range(vma, old_addr, old_end);

	for (; old_addr < old_end; old_addr += extent, new_addr += extent) {
		cond_resched();
		next = (old_addr + PMD_SIZE) & PMD_MASK;
		if (next - 1 > old_end)
			next = old_end;
		extent = next - old_addr;
		old_pmd = get_old_pmd(vma->vm_mm, old_addr);
		if (!old_pmd)
			continue;
		new_pmd = alloc_new_pmd(vma->vm_mm, new_addr);
		if (!new_pmd)
			break;
		next = (new_addr + PMD_SIZE) & PMD_MASK;
		if (extent > next - new_addr)
			extent = next - new_addr;
		if (extent > LATENCY_LIMIT)
			extent = LATENCY_LIMIT;
		move_ptes(vma, old_pmd, old_addr, old_addr + extent,
				new_vma, new_pmd, new_addr);
	}

	return len + old_addr - old_end;	/* how much done */
}