static int nf_tables_delsetelem(struct sk_buff *skb,
				const struct nfnl_info *info,
				const struct nlattr * const nla[])
{
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	u8 family = info->nfmsg->nfgen_family;
	struct net *net = info->net;
	const struct nlattr *attr;
	struct nft_table *table;
	struct nft_set *set;
	struct nft_ctx ctx;
	int rem, err = 0;

	table = nft_table_lookup(net, nla[NFTA_SET_ELEM_LIST_TABLE], family,
				 genmask, NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_SET_ELEM_LIST_TABLE]);
		return PTR_ERR(table);
	}

	set = nft_set_lookup(table, nla[NFTA_SET_ELEM_LIST_SET], genmask);
	if (IS_ERR(set))
		return PTR_ERR(set);
	if (!list_empty(&set->bindings) && set->flags & NFT_SET_CONSTANT)
		return -EBUSY;

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

	if (!nla[NFTA_SET_ELEM_LIST_ELEMENTS])
		return nft_set_flush(&ctx, set, genmask);

	nla_for_each_nested(attr, nla[NFTA_SET_ELEM_LIST_ELEMENTS], rem) {
		err = nft_del_setelem(&ctx, set, attr);
		if (err < 0) {
			NL_SET_BAD_ATTR(extack, attr);
			break;
		}
	}
	return err;
}