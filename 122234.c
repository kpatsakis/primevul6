static inline int pmdp_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long address,
					    pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;
	int r = 1;
	if (!pmd_young(pmd))
		r = 0;
	else
		set_pmd_at(vma->vm_mm, address, pmdp, pmd_mkold(pmd));
	return r;
}