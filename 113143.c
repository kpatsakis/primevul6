static inline unsigned long mm_pgtables_bytes(const struct mm_struct *mm)
{
	return atomic_long_read(&mm->pgtables_bytes);
}