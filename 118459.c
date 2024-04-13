u32 gf_m2ts_pes_get_framing_mode(GF_M2TS_PES *pes)
{
	if (pes->flags & GF_M2TS_ES_IS_SECTION) {
		if (pes->flags & GF_M2TS_ES_IS_SL) {
			if ( ((GF_M2TS_SECTION_ES *)pes)->sec->process_section == NULL)
				return GF_M2TS_PES_FRAMING_DEFAULT;

		}
		return GF_M2TS_PES_FRAMING_SKIP_NO_RESET;
	}

	if (!pes->reframe ) return GF_M2TS_PES_FRAMING_SKIP_NO_RESET;
	if (pes->reframe == gf_m2ts_reframe_default) return GF_M2TS_PES_FRAMING_RAW;
	if (pes->reframe == gf_m2ts_reframe_reset) return GF_M2TS_PES_FRAMING_SKIP;
	return GF_M2TS_PES_FRAMING_DEFAULT;
}