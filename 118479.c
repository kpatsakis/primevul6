static u32 gf_m2ts_sync(GF_M2TS_Demuxer *ts, char *data, u32 size, Bool simple_check)
{
	u32 i=0;
	/*if first byte is sync assume we're sync*/
	if (simple_check && (data[i]==0x47)) return 0;

	while (i < size) {
		if (i+192 >= size) return size;
		if ((data[i]==0x47) && (data[i+188]==0x47))
			break;
		if (i+192 >= size) return size;
		if ((data[i]==0x47) && (data[i+192]==0x47)) {
			ts->prefix_present = 1;
			break;
		}
		i++;
	}
	if (i) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] re-sync skipped %d bytes\n", i) );
	}
	return i;
}