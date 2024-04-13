bool isolate_huge_page(struct page *page, struct list_head *list)
{
	bool ret = true;

	spin_lock_irq(&hugetlb_lock);
	if (!PageHeadHuge(page) ||
	    !HPageMigratable(page) ||
	    !get_page_unless_zero(page)) {
		ret = false;
		goto unlock;
	}
	ClearHPageMigratable(page);
	list_move_tail(&page->lru, list);
unlock:
	spin_unlock_irq(&hugetlb_lock);
	return ret;
}