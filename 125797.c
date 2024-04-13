static int nf_tables_newsetelem(struct sk_buff *skb,
				const struct nfnl_info *info,
				const struct nlattr * const nla[])
{
	struct nftables_pernet *nft_net = nft_pernet(info->net);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	u8 family = info->nfmsg->nfgen_family;
	struct net *net = info->net;
	const struct nlattr *attr;
	struct nft_table *table;
	struct nft_set *set;
	struct nft_ctx ctx;
	int rem, err;

	if (nla[NFTA_SET_ELEM_LIST_ELEMENTS] == NULL)
		return -EINVAL;

	table = nft_table_lookup(net, nla[NFTA_SET_ELEM_LIST_TABLE], family,
				 genmask, NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_SET_ELEM_LIST_TABLE]);
		return PTR_ERR(table);
	}

	set = nft_set_lookup_global(net, table, nla[NFTA_SET_ELEM_LIST_SET],
				    nla[NFTA_SET_ELEM_LIST_SET_ID], genmask);
	if (IS_ERR(set))
		return PTR_ERR(set);

	if (!list_empty(&set->bindings) && set->flags & NFT_SET_CONSTANT)
		return -EBUSY;

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, NULL, nla);

	nla_for_each_nested(attr, nla[NFTA_SET_ELEM_LIST_ELEMENTS], rem) {
		err = nft_add_set_elem(&ctx, set, attr, info->nlh->nlmsg_flags);
		if (err < 0) {
			NL_SET_BAD_ATTR(extack, attr);
			return err;
		}
	}

	if (nft_net->validate_state == NFT_VALIDATE_DO)
		return nft_table_validate(net, table);

	return 0;
}