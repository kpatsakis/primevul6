static inline unsigned long get_mm_hiwater_vm(struct mm_struct *mm)
{
	return max(mm->hiwater_vm, mm->total_vm);
}