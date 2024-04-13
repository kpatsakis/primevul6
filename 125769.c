void nft_reg_track_update(struct nft_regs_track *track,
			  const struct nft_expr *expr, u8 dreg, u8 len)
{
	unsigned int regcount;
	int i;

	__nft_reg_track_clobber(track, dreg);

	regcount = DIV_ROUND_UP(len, NFT_REG32_SIZE);
	for (i = 0; i < regcount; i++, dreg++)
		__nft_reg_track_update(track, expr, dreg, i);
}