static inline void inc_mm_counter(struct mm_struct *mm, int member)
{
	atomic_long_inc(&mm->rss_stat.count[member]);
}