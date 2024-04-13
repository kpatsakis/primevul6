
GF_Err clap_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_CleanApertureBox *ptr = (GF_CleanApertureBox*)s;
	ISOM_DECREASE_SIZE(ptr, 32);
	ptr->cleanApertureWidthN = gf_bs_read_u32(bs);
	ptr->cleanApertureWidthD = gf_bs_read_u32(bs);
	ptr->cleanApertureHeightN = gf_bs_read_u32(bs);
	ptr->cleanApertureHeightD = gf_bs_read_u32(bs);
	ptr->horizOffN = gf_bs_read_u32(bs);
	ptr->horizOffD = gf_bs_read_u32(bs);
	ptr->vertOffN = gf_bs_read_u32(bs);
	ptr->vertOffD = gf_bs_read_u32(bs);
	return GF_OK;