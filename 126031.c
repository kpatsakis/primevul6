static ssize_t demote_size_show(struct kobject *kobj,
					struct kobj_attribute *attr, char *buf)
{
	int nid;
	struct hstate *h = kobj_to_hstate(kobj, &nid);
	unsigned long demote_size = (PAGE_SIZE << h->demote_order) / SZ_1K;

	return sysfs_emit(buf, "%lukB\n", demote_size);
}