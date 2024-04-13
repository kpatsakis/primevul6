static int hugetlb_acct_memory(struct hstate *h, long delta)
{
	int ret = -ENOMEM;

	if (!delta)
		return 0;

	spin_lock_irq(&hugetlb_lock);
	/*
	 * When cpuset is configured, it breaks the strict hugetlb page
	 * reservation as the accounting is done on a global variable. Such
	 * reservation is completely rubbish in the presence of cpuset because
	 * the reservation is not checked against page availability for the
	 * current cpuset. Application can still potentially OOM'ed by kernel
	 * with lack of free htlb page in cpuset that the task is in.
	 * Attempt to enforce strict accounting with cpuset is almost
	 * impossible (or too ugly) because cpuset is too fluid that
	 * task or memory node can be dynamically moved between cpusets.
	 *
	 * The change of semantics for shared hugetlb mapping with cpuset is
	 * undesirable. However, in order to preserve some of the semantics,
	 * we fall back to check against current free page availability as
	 * a best attempt and hopefully to minimize the impact of changing
	 * semantics that cpuset has.
	 *
	 * Apart from cpuset, we also have memory policy mechanism that
	 * also determines from which node the kernel will allocate memory
	 * in a NUMA system. So similar to cpuset, we also should consider
	 * the memory policy of the current task. Similar to the description
	 * above.
	 */
	if (delta > 0) {
		if (gather_surplus_pages(h, delta) < 0)
			goto out;

		if (delta > allowed_mems_nr(h)) {
			return_unused_surplus_pages(h, delta);
			goto out;
		}
	}

	ret = 0;
	if (delta < 0)
		return_unused_surplus_pages(h, (unsigned long) -delta);

out:
	spin_unlock_irq(&hugetlb_lock);
	return ret;
}