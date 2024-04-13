static inline void mm_dec_nr_puds(struct mm_struct *mm)
{
	if (mm_pud_folded(mm))
		return;
	atomic_long_sub(PTRS_PER_PUD * sizeof(pud_t), &mm->pgtables_bytes);
}