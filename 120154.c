
GF_Err prft_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_ProducerReferenceTimeBox *ptr = (GF_ProducerReferenceTimeBox *) s;

	ptr->refTrackID = gf_bs_read_u32(bs);
	ptr->ntp = gf_bs_read_u64(bs);
	if (ptr->version==0) {
		ptr->timestamp = gf_bs_read_u32(bs);
	} else {
		ptr->timestamp = gf_bs_read_u64(bs);
	}
	return GF_OK;