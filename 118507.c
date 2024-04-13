void gf_m2ts_demux_dmscc_init(GF_M2TS_Demuxer *ts) {

	char temp_dir[GF_MAX_PATH];
	u32 length;
	GF_Err e;

	ts->dsmcc_controler = gf_list_new();
	ts->process_dmscc = 1;

	strcpy(temp_dir, gf_get_default_cache_directory() );
	length = (u32) strlen(temp_dir);
	if(temp_dir[length-1] == GF_PATH_SEPARATOR) {
		temp_dir[length-1] = 0;
	}

	ts->dsmcc_root_dir = (char*)gf_calloc(strlen(temp_dir)+strlen("CarouselData")+2,sizeof(char));
	sprintf(ts->dsmcc_root_dir,"%s%cCarouselData",temp_dir,GF_PATH_SEPARATOR);
	e = gf_mkdir(ts->dsmcc_root_dir);
	if(e) {
		GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[Process DSMCC] Error during the creation of the directory %s \n",ts->dsmcc_root_dir));
	}

}