static struct nft_rule_blob *nf_tables_chain_alloc_rules(unsigned int size)
{
	struct nft_rule_blob *blob;

	/* size must include room for the last rule */
	if (size < offsetof(struct nft_rule_dp, data))
		return NULL;

	size += sizeof(struct nft_rule_blob) + sizeof(struct nft_rules_old);
	if (size > INT_MAX)
		return NULL;

	blob = kvmalloc(size, GFP_KERNEL_ACCOUNT);
	if (!blob)
		return NULL;

	blob->size = 0;
	nft_last_rule(blob, blob->data);

	return blob;
}