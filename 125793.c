static int nf_tables_commit_chain_prepare(struct net *net, struct nft_chain *chain)
{
	const struct nft_expr *expr, *last;
	struct nft_regs_track track = {};
	unsigned int size, data_size;
	void *data, *data_boundary;
	struct nft_rule_dp *prule;
	struct nft_rule *rule;

	/* already handled or inactive chain? */
	if (chain->blob_next || !nft_is_active_next(net, chain))
		return 0;

	data_size = 0;
	list_for_each_entry(rule, &chain->rules, list) {
		if (nft_is_active_next(net, rule)) {
			data_size += sizeof(*prule) + rule->dlen;
			if (data_size > INT_MAX)
				return -ENOMEM;
		}
	}
	data_size += offsetof(struct nft_rule_dp, data);	/* last rule */

	chain->blob_next = nf_tables_chain_alloc_rules(data_size);
	if (!chain->blob_next)
		return -ENOMEM;

	data = (void *)chain->blob_next->data;
	data_boundary = data + data_size;
	size = 0;

	list_for_each_entry(rule, &chain->rules, list) {
		if (!nft_is_active_next(net, rule))
			continue;

		prule = (struct nft_rule_dp *)data;
		data += offsetof(struct nft_rule_dp, data);
		if (WARN_ON_ONCE(data > data_boundary))
			return -ENOMEM;

		size = 0;
		track.last = nft_expr_last(rule);
		nft_rule_for_each_expr(expr, last, rule) {
			track.cur = expr;

			if (nft_expr_reduce(&track, expr)) {
				expr = track.cur;
				continue;
			}

			if (WARN_ON_ONCE(data + expr->ops->size > data_boundary))
				return -ENOMEM;

			memcpy(data + size, expr, expr->ops->size);
			size += expr->ops->size;
		}
		if (WARN_ON_ONCE(size >= 1 << 12))
			return -ENOMEM;

		prule->handle = rule->handle;
		prule->dlen = size;
		prule->is_last = 0;

		data += size;
		size = 0;
		chain->blob_next->size += (unsigned long)(data - (void *)prule);
	}

	prule = (struct nft_rule_dp *)data;
	data += offsetof(struct nft_rule_dp, data);
	if (WARN_ON_ONCE(data > data_boundary))
		return -ENOMEM;

	nft_last_rule(chain->blob_next, prule);

	return 0;
}