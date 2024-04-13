static inline void ptep_modify_prot_commit(struct vm_area_struct *vma, unsigned long addr,
					   pte_t *ptep, pte_t old_pte, pte_t pte)
{

	if (sizeof(pteval_t) > sizeof(long))
		/* 5 arg words */
		pv_ops.mmu.ptep_modify_prot_commit(vma, addr, ptep, pte);
	else
		PVOP_VCALL4(mmu.ptep_modify_prot_commit,
			    vma, addr, ptep, pte.pte);
}