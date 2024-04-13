void gf_m2ts_set_segment_switch(GF_M2TS_Demuxer *ts)
{
	u32 i;
	for (i=0; i<GF_M2TS_MAX_STREAMS; i++) {
		GF_M2TS_ES *es = (GF_M2TS_ES *) ts->ess[i];
		if (!es) continue;
		es->flags |= GF_M2TS_ES_IGNORE_NEXT_DISCONTINUITY;
	}
}