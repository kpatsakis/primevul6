static inline void mm_pgtables_bytes_init(struct mm_struct *mm)
{
	atomic_long_set(&mm->pgtables_bytes, 0);
}