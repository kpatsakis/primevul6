
void nhml_get_bs(GF_BitStream **bs, char *data, u32 size, u32 mode)
{
	if (*bs) gf_bs_reassign_buffer(*bs, data, size);
	else  (*bs) = gf_bs_new(data, size, mode);