static int hugetlb_sysfs_add_hstate(struct hstate *h, struct kobject *parent,
				    struct kobject **hstate_kobjs,
				    const struct attribute_group *hstate_attr_group)
{
	int retval;
	int hi = hstate_index(h);

	hstate_kobjs[hi] = kobject_create_and_add(h->name, parent);
	if (!hstate_kobjs[hi])
		return -ENOMEM;

	retval = sysfs_create_group(hstate_kobjs[hi], hstate_attr_group);
	if (retval) {
		kobject_put(hstate_kobjs[hi]);
		hstate_kobjs[hi] = NULL;
	}

	if (h->demote_order) {
		if (sysfs_create_group(hstate_kobjs[hi],
					&hstate_demote_attr_group))
			pr_warn("HugeTLB unable to create demote interfaces for %s\n", h->name);
	}

	return retval;
}