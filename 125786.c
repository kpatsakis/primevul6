static void __nft_reg_track_clobber(struct nft_regs_track *track, u8 dreg)
{
	int i;

	for (i = track->regs[dreg].num_reg; i > 0; i--)
		__nft_reg_track_cancel(track, dreg - i);
}