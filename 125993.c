static int proc_hugetlb_doulongvec_minmax(struct ctl_table *table, int write,
					  void *buffer, size_t *length,
					  loff_t *ppos, unsigned long *out)
{
	struct ctl_table dup_table;

	/*
	 * In order to avoid races with __do_proc_doulongvec_minmax(), we
	 * can duplicate the @table and alter the duplicate of it.
	 */
	dup_table = *table;
	dup_table.data = out;

	return proc_doulongvec_minmax(&dup_table, write, buffer, length, ppos);
}