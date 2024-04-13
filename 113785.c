
GF_Err nhmldmx_process(GF_Filter *filter)
{
	GF_NHMLDmxCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck;
	GF_Err e;
	Bool start, end;

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (pck) {
		gf_filter_pck_get_framing(pck, &start, &end);
		//for now we only work with complete files
		assert(end);
	}


	//need init ?
	switch (ctx->parsing_state) {
	case 0:
		e = nhmldmx_init_parsing(filter, ctx);
		if (e) {
			ctx->parsing_state = 3;
			return e;
		}
		ctx->parsing_state = 1;
		//fall-through
	case 1:
		if (!ctx->is_playing) return GF_OK;

		e = nhmldmx_send_sample(filter, ctx);
		if (e) return e;
		break;
	case 2:
	default:
		if (pck) gf_filter_pid_drop_packet(ctx->ipid);
		if (ctx->opid) {
			gf_filter_pid_set_eos(ctx->opid);
			return GF_EOS;
		}
		break;
	}
	return GF_OK;