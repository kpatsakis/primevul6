GF_Err cslg_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_CompositionToDecodeBox *ptr = (GF_CompositionToDecodeBox *)s;

	ptr->compositionToDTSShift = gf_bs_read_int(bs, 32);
	ptr->leastDecodeToDisplayDelta = gf_bs_read_int(bs, 32);
	ptr->greatestDecodeToDisplayDelta = gf_bs_read_int(bs, 32);
	ptr->compositionStartTime = gf_bs_read_int(bs, 32);
	ptr->compositionEndTime = gf_bs_read_int(bs, 32);
	return GF_OK;
}