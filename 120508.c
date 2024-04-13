
GF_Err txtc_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextConfigBox *ptr = (GF_TextConfigBox*)s;
	ptr->config = (char *)gf_malloc(sizeof(char)*((u32) ptr->size+1));
	gf_bs_read_data(bs, ptr->config, (u32) ptr->size);
	ptr->config[ptr->size] = 0;
	return GF_OK;