static int nft_set_desc_concat_parse(const struct nlattr *attr,
				     struct nft_set_desc *desc)
{
	struct nlattr *tb[NFTA_SET_FIELD_MAX + 1];
	u32 len;
	int err;

	if (desc->field_count >= ARRAY_SIZE(desc->field_len))
		return -E2BIG;

	err = nla_parse_nested_deprecated(tb, NFTA_SET_FIELD_MAX, attr,
					  nft_concat_policy, NULL);
	if (err < 0)
		return err;

	if (!tb[NFTA_SET_FIELD_LEN])
		return -EINVAL;

	len = ntohl(nla_get_be32(tb[NFTA_SET_FIELD_LEN]));
	if (!len || len > U8_MAX)
		return -EINVAL;

	desc->field_len[desc->field_count++] = len;

	return 0;
}