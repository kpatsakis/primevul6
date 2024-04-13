int AVI_read_audio(avi_t *AVI, u8 *audbuf, int bytes, int *continuous)
{
	int nr, todo;
	s64 pos;

	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->track[AVI->aptr].audio_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	nr = 0; /* total number of bytes read */

	if (bytes==0) {
		AVI->track[AVI->aptr].audio_posc++;
		AVI->track[AVI->aptr].audio_posb = 0;
	}

	*continuous = 1;
	while(bytes>0)
	{
		s64 ret;
		int left = (int) (AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len - AVI->track[AVI->aptr].audio_posb);
		if(left==0)
		{
			if(AVI->track[AVI->aptr].audio_posc>=AVI->track[AVI->aptr].audio_chunks-1) return nr;
			AVI->track[AVI->aptr].audio_posc++;
			AVI->track[AVI->aptr].audio_posb = 0;
			*continuous = 0;
			continue;
		}
		if(bytes<left)
			todo = bytes;
		else
			todo = left;
		pos = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].pos + AVI->track[AVI->aptr].audio_posb;
		gf_fseek(AVI->fdes, pos, SEEK_SET);
		if ( (ret = avi_read(AVI->fdes,audbuf+nr,todo)) != todo)
		{
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[avilib] XXX pos = %"LLD", ret = %"LLD", todo = %ld\n", pos, ret, todo));
			AVI_errno = AVI_ERR_READ;
			return -1;
		}
		bytes -= todo;
		nr    += todo;
		AVI->track[AVI->aptr].audio_posb += todo;
	}

	return nr;
}