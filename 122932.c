GF_Err gf_bs_set_logger(GF_BitStream *bs, void (*on_bs_log)(void *udta, const char *field_name, u32 nb_bits, u64 field_val, s32 idx1, s32 idx2, s32 idx3), void *udta)
{
	if (!bs) return GF_BAD_PARAM;
	bs->on_log = on_bs_log;
	bs->log_udta = udta;
	return GF_OK;
}