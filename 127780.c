char *ppp_dev_name(struct ppp_channel *chan)
{
	struct channel *pch = chan->ppp;
	char *name = NULL;

	if (pch) {
		read_lock_bh(&pch->upl);
		if (pch->ppp && pch->ppp->dev)
			name = pch->ppp->dev->name;
		read_unlock_bh(&pch->upl);
	}
	return name;
}