static inline void mm_dec_nr_ptes(struct mm_struct *mm)
{
	atomic_long_sub(PTRS_PER_PTE * sizeof(pte_t), &mm->pgtables_bytes);
}