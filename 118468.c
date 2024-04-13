static u32 gf_m2ts_reframe_id3_pes(GF_M2TS_Demuxer *ts, GF_M2TS_PES *pes, Bool same_pts, unsigned char *data, u32 data_len, GF_M2TS_PESHeader *pes_hdr)
{
	char frame_header[256];
	char *output_text = NULL;
	u32 output_len = 0;
	u32 pos = 0;
	GF_M2TS_PES_PCK pck;
	pck.flags = 0;
	if (pes->rap) pck.flags |= GF_M2TS_PES_PCK_RAP;
	if (!same_pts) pck.flags |= GF_M2TS_PES_PCK_AU_START;
	pck.DTS = pes->DTS;
	pck.PTS = pes->PTS;
	sprintf(frame_header, LLU" --> NEXT\n", pes->PTS);
	add_text(&output_text, &output_len, &pos, frame_header, (u32)strlen(frame_header));
	id3_parse_tag((char *)data, data_len, &output_text, &output_len, &pos);
	add_text(&output_text, &output_len, &pos, "\n\n", 2);
	pck.data = (char *)output_text;
	pck.data_len = pos;
	pck.stream = pes;
	ts->on_event(ts, GF_M2TS_EVT_PES_PCK, &pck);
	gf_free(output_text);
	/*we consumed all data*/
	return 0;
}