void gf_m2ts_print_info(GF_M2TS_Demuxer *ts)
{
#ifdef GPAC_ENABLE_MPE
	gf_m2ts_print_mpe_info(ts);
#endif
}