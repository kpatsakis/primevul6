static inline void paravirt_arch_exit_mmap(struct mm_struct *mm)
{
	PVOP_VCALL1(mmu.exit_mmap, mm);
}