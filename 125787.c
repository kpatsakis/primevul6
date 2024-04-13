static void __nft_reg_track_update(struct nft_regs_track *track,
				   const struct nft_expr *expr,
				   u8 dreg, u8 num_reg)
{
	track->regs[dreg].selector = expr;
	track->regs[dreg].bitwise = NULL;
	track->regs[dreg].num_reg = num_reg;
}