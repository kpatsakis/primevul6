static void naludmx_finalize_au_flags(GF_NALUDmxCtx *ctx)
{
	u64 ts;
	Bool is_rap = GF_FALSE;

	if (!ctx->first_pck_in_au)
		return;
	if (ctx->au_sap) {
		gf_filter_pck_set_sap(ctx->first_pck_in_au, ctx->au_sap);
		if (ctx->au_sap == GF_FILTER_SAP_1) {
			ctx->dts_last_IDR = gf_filter_pck_get_dts(ctx->first_pck_in_au);
			if (ctx->is_paff)
				ctx->dts_last_IDR *= 2;
		}
		if (ctx->au_sap <= GF_FILTER_SAP_3) {
			is_rap = GF_TRUE;
		}
	}
	else if (ctx->has_islice && ctx->force_sync && (ctx->sei_recovery_frame_count==0)) {
		gf_filter_pck_set_sap(ctx->first_pck_in_au, GF_FILTER_SAP_1);
		if (!ctx->use_opengop_gdr) {
			ctx->use_opengop_gdr = 1;
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[%s] Forcing non-IDR samples with I slices to be marked as sync points - resulting file will not be ISOBMFF conformant\n", ctx->log_name));
		}
		is_rap = GF_TRUE;
	}
	/*set roll info sampleGroups info*/
	else if (!ctx->au_sap && ( (ctx->sei_recovery_frame_count >= 0) || ctx->has_islice) ) {
		/*generic GDR*/
		if (ctx->sei_recovery_frame_count > 0) {
			if (!ctx->use_opengop_gdr) ctx->use_opengop_gdr = 1;
			gf_filter_pck_set_sap(ctx->first_pck_in_au, GF_FILTER_SAP_4);
			gf_filter_pck_set_roll_info(ctx->first_pck_in_au, ctx->sei_recovery_frame_count);
		}
		/*open-GOP*/
		else if ((ctx->sei_recovery_frame_count == 0) && ctx->has_islice) {
			if (!ctx->use_opengop_gdr) ctx->use_opengop_gdr = 2;
			gf_filter_pck_set_sap(ctx->first_pck_in_au, GF_FILTER_SAP_3);
			is_rap = GF_TRUE;
		}
	}
	if (ctx->is_paff) {
		gf_filter_pck_set_interlaced(ctx->first_pck_in_au, ctx->bottom_field_flag ? 2 : 1);
	}

	//if TS is set, the packet was the first in AU in the input timed packet (eg PES), we reuse the input timing
	ts = gf_filter_pck_get_cts(ctx->first_pck_in_au);
	if (ts == GF_FILTER_NO_TS) {
		/*we store the POC (last POC minus the poc shift) as the CTS offset and re-update the CTS when dispatching*/
		assert(ctx->last_poc >= ctx->poc_shift);
		gf_filter_pck_set_cts(ctx->first_pck_in_au, CTS_POC_OFFSET_SAFETY + ctx->last_poc - ctx->poc_shift);
		//we use the carousel flag temporarly to indicate the cts must be recomputed
		gf_filter_pck_set_carousel_version(ctx->first_pck_in_au, 1);
	}

	if (ctx->subsamp_buffer_size) {
		gf_filter_pck_set_property(ctx->first_pck_in_au, GF_PROP_PCK_SUBS, &PROP_DATA(ctx->subsamp_buffer, ctx->subsamp_buffer_size) );
		ctx->subsamp_buffer_size = 0;
		ctx->subs_mapped_bytes = 0;
	}
	if (ctx->deps) {
		u8 flags = 0;
		//dependsOn
		flags = (is_rap) ? 2 : 1;
		flags <<= 2;
		//dependedOn
	 	flags |= ctx->has_ref_slices ? 1 : 2;
		flags <<= 2;
		//hasRedundant
	 	flags |= ctx->has_redundant ? 1 : 2;
	 	gf_filter_pck_set_dependency_flags(ctx->first_pck_in_au, flags);
	}
	ctx->has_ref_slices = GF_FALSE;
	ctx->has_redundant = GF_FALSE;

	//if we reuse input packets timing, we can dispatch asap.
	//otherwise if poc probe is done (we know the min_poc_diff between images) and we are not in strict mode, dispatch asap
	//otherwise we will need to wait for the next ref frame to make sure we know all pocs ...
	if (ctx->timescale || (!ctx->strict_poc && ctx->poc_probe_done) )
		naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);

	ctx->first_pck_in_au = NULL;
}