static int rt6_bind_neighbour(struct rt6_info *rt, struct net_device *dev)
{
	struct neighbour *n = __ipv6_neigh_lookup(&nd_tbl, dev, &rt->rt6i_gateway);
	if (!n) {
		n = neigh_create(&nd_tbl, &rt->rt6i_gateway, dev);
		if (IS_ERR(n))
			return PTR_ERR(n);
	}
	rt->n = n;

	return 0;
}