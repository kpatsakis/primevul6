void gf_media_get_reduced_frame_rate(u32 *timescale, u32 *sample_dur)
{
	u32 res;
	if (!*sample_dur) return;
	res = *timescale / *sample_dur;
	if (res * (*sample_dur) == *timescale) {
		*timescale = res;
		*sample_dur = 1;
	}
	else if ((double)(*timescale * 1001 - (res + 1) * *sample_dur * 1000) / ((res + 1) * *sample_dur * 1000) < 0.001) {
		*timescale = (res + 1) * 1000;
		*sample_dur = 1001;
	}
}