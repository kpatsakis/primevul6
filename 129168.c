static inline void verify_mm_writelocked(struct mm_struct *mm)
{
#ifdef CONFIG_DEBUG_VM
	if (unlikely(down_read_trylock(&mm->mmap_sem))) {
		WARN_ON(1);
		up_read(&mm->mmap_sem);
	}
#endif
}