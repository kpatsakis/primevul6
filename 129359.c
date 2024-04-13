static struct rt6_info *find_match(struct rt6_info *rt, int oif, int strict,
				   int *mpri, struct rt6_info *match)
{
	int m;

	if (rt6_check_expired(rt))
		goto out;

	m = rt6_score_route(rt, oif, strict);
	if (m < 0)
		goto out;

	if (m > *mpri) {
		if (strict & RT6_LOOKUP_F_REACHABLE)
			rt6_probe(match);
		*mpri = m;
		match = rt;
	} else if (strict & RT6_LOOKUP_F_REACHABLE) {
		rt6_probe(rt);
	}

out:
	return match;
}