static inline void mm_dec_nr_pmds(struct mm_struct *mm)
{
	if (mm_pmd_folded(mm))
		return;
	atomic_long_sub(PTRS_PER_PMD * sizeof(pmd_t), &mm->pgtables_bytes);
}