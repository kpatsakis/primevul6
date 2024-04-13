GF_Err naludmx_realloc_last_pck(GF_NALUDmxCtx *ctx, u32 nb_bytes_to_add, u8 **data_ptr)
{
	GF_Err e;
	u8 *pck_data;
	u32 full_size;
	GF_FilterPacket *pck = gf_list_last(ctx->pck_queue);
	*data_ptr = NULL;
	if (!pck) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[%s] attempt to reallocate a non-existing packet!\n", ctx->log_name));
		return GF_SERVICE_ERROR;
	}
	e = gf_filter_pck_expand(pck, nb_bytes_to_add, &pck_data, data_ptr, &full_size);
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[%s] Failed to reallocate packet buffer: %s\n", ctx->log_name, gf_error_to_string(e) ));
		return e;
	}
	assert(ctx->bs_w);
	//rewrite NALU size length
	full_size -= ctx->nal_length;
	gf_bs_reassign_buffer(ctx->bs_w, pck_data, ctx->nal_length);
	gf_bs_write_int(ctx->bs_w, full_size, 8*ctx->nal_length);
	naludmx_update_nalu_maxsize(ctx, full_size);
	//rewrite subsample size
	if (ctx->subsamples) {
		assert(ctx->subsamp_buffer_size>=14);
		//reassign to beginning of size field (after first u32 flags)
		gf_bs_reassign_buffer(ctx->bs_w, ctx->subsamp_buffer + ctx->subsamp_buffer_size-14 + 4, 14 - 4);
		gf_bs_write_u32(ctx->bs_w, full_size + ctx->nal_length);
	}
	return GF_OK;
}