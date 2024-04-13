static int nft_set_desc_concat(struct nft_set_desc *desc,
			       const struct nlattr *nla)
{
	struct nlattr *attr;
	u32 num_regs = 0;
	int rem, err, i;

	nla_for_each_nested(attr, nla, rem) {
		if (nla_type(attr) != NFTA_LIST_ELEM)
			return -EINVAL;

		err = nft_set_desc_concat_parse(attr, desc);
		if (err < 0)
			return err;
	}

	for (i = 0; i < desc->field_count; i++)
		num_regs += DIV_ROUND_UP(desc->field_len[i], sizeof(u32));

	if (num_regs > NFT_REG32_COUNT)
		return -E2BIG;

	return 0;
}