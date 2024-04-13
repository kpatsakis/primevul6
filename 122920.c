GF_BitStream *gf_bs_new_cbk(GF_Err (*on_block_out)(void *cbk, u8 *data, u32 block_size), void *usr_data, u32 block_size)
{
	return gf_bs_new_cbk_buffer(on_block_out, usr_data, NULL, block_size);

}