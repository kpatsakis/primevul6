static void icmp6_clean_all(int (*func)(struct rt6_info *rt, void *arg),
			    void *arg)
{
	struct dst_entry *dst, **pprev;

	spin_lock_bh(&icmp6_dst_lock);
	pprev = &icmp6_dst_gc_list;
	while ((dst = *pprev) != NULL) {
		struct rt6_info *rt = (struct rt6_info *) dst;
		if (func(rt, arg)) {
			*pprev = dst->next;
			dst_free(dst);
		} else {
			pprev = &dst->next;
		}
	}
	spin_unlock_bh(&icmp6_dst_lock);
}