static int rt6_info_hash_nhsfn(unsigned int candidate_count,
			       const struct flowi6 *fl6)
{
	unsigned int val = fl6->flowi6_proto;

	val ^= fl6->daddr.s6_addr32[0];
	val ^= fl6->daddr.s6_addr32[1];
	val ^= fl6->daddr.s6_addr32[2];
	val ^= fl6->daddr.s6_addr32[3];

	val ^= fl6->saddr.s6_addr32[0];
	val ^= fl6->saddr.s6_addr32[1];
	val ^= fl6->saddr.s6_addr32[2];
	val ^= fl6->saddr.s6_addr32[3];

	/* Work only if this not encapsulated */
	switch (fl6->flowi6_proto) {
	case IPPROTO_UDP:
	case IPPROTO_TCP:
	case IPPROTO_SCTP:
		val ^= fl6->fl6_sport;
		val ^= fl6->fl6_dport;
		break;

	case IPPROTO_ICMPV6:
		val ^= fl6->fl6_icmp_type;
		val ^= fl6->fl6_icmp_code;
		break;
	}
	/* RFC6438 recommands to use flowlabel */
	val ^= fl6->flowlabel;

	/* Perhaps, we need to tune, this function? */
	val = val ^ (val >> 7) ^ (val >> 12);
	return val % candidate_count;
}