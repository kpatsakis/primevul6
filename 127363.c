static inline pte_t ptep_modify_prot_start(struct vm_area_struct *vma, unsigned long addr,
					   pte_t *ptep)
{
	pteval_t ret;

	ret = PVOP_CALL3(pteval_t, mmu.ptep_modify_prot_start, vma, addr, ptep);

	return (pte_t) { .pte = ret };
}