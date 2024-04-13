static inline void dec_mm_counter(struct mm_struct *mm, int member)
{
	atomic_long_dec(&mm->rss_stat.count[member]);
}