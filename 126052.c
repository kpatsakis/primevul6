static inline bool subpool_is_free(struct hugepage_subpool *spool)
{
	if (spool->count)
		return false;
	if (spool->max_hpages != -1)
		return spool->used_hpages == 0;
	if (spool->min_hpages != -1)
		return spool->rsv_hpages == spool->min_hpages;

	return true;
}