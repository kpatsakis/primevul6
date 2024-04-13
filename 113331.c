static inline void update_hiwater_vm(struct mm_struct *mm)
{
	if (mm->hiwater_vm < mm->total_vm)
		mm->hiwater_vm = mm->total_vm;
}