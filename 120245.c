GF_Err hmhd_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_HintMediaHeaderBox *ptr = (GF_HintMediaHeaderBox *)s;

	ptr->maxPDUSize = gf_bs_read_u16(bs);
	ptr->avgPDUSize = gf_bs_read_u16(bs);
	ptr->maxBitrate = gf_bs_read_u32(bs);
	ptr->avgBitrate = gf_bs_read_u32(bs);
	ptr->slidingAverageBitrate = gf_bs_read_u32(bs);
	return GF_OK;
}