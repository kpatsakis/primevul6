static inline void add_mm_counter(struct mm_struct *mm, int member, long value)
{
	atomic_long_add(value, &mm->rss_stat.count[member]);
}