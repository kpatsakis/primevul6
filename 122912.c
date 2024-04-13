void gf_bs_prevent_dispatch(GF_BitStream *bs, Bool prevent_dispatch)
{
	if (!bs) return;
	if (prevent_dispatch) {
		bs->prevent_dispatch ++;
		return;
	}
	if (!bs->prevent_dispatch) return;
	bs->prevent_dispatch --;

	if (bs->on_block_out && !bs->prevent_dispatch) {
		assert(bs->position >= bs->bytes_out);
		if (bs->position > bs->bytes_out) {
			bs->on_block_out(bs->usr_data, bs->original, (u32) (bs->position - bs->bytes_out));
			bs->bytes_out = bs->position;
		}
	}
}