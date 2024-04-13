static bool rt6_check_expired(const struct rt6_info *rt)
{
	if (rt->rt6i_flags & RTF_EXPIRES) {
		if (time_after(jiffies, rt->dst.expires))
			return true;
	} else if (rt->dst.from) {
		return rt6_check_expired((struct rt6_info *) rt->dst.from);
	}
	return false;
}