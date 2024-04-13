static inline void reset_mm_hiwater_rss(struct mm_struct *mm)
{
	mm->hiwater_rss = get_mm_rss(mm);
}