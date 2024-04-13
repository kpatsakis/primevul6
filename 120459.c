
GF_Err ainf_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_AssetInformationBox *ptr = (GF_AssetInformationBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->profile_version);
	gf_bs_write_data(bs, ptr->APID, (u32) strlen(ptr->APID) + 1);
	return GF_OK;