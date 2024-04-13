static struct dst_entry *ip6_negative_advice(struct dst_entry *dst)
{
	struct rt6_info *rt = (struct rt6_info *) dst;

	if (rt) {
		if (rt->rt6i_flags & RTF_CACHE) {
			if (rt6_check_expired(rt)) {
				ip6_del_rt(rt);
				dst = NULL;
			}
		} else {
			dst_release(dst);
			dst = NULL;
		}
	}
	return dst;
}