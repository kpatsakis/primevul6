static ssize_t demote_size_store(struct kobject *kobj,
					struct kobj_attribute *attr,
					const char *buf, size_t count)
{
	struct hstate *h, *demote_hstate;
	unsigned long demote_size;
	unsigned int demote_order;
	int nid;

	demote_size = (unsigned long)memparse(buf, NULL);

	demote_hstate = size_to_hstate(demote_size);
	if (!demote_hstate)
		return -EINVAL;
	demote_order = demote_hstate->order;
	if (demote_order < HUGETLB_PAGE_ORDER)
		return -EINVAL;

	/* demote order must be smaller than hstate order */
	h = kobj_to_hstate(kobj, &nid);
	if (demote_order >= h->order)
		return -EINVAL;

	/* resize_lock synchronizes access to demote size and writes */
	mutex_lock(&h->resize_lock);
	h->demote_order = demote_order;
	mutex_unlock(&h->resize_lock);

	return count;
}