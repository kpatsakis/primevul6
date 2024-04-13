int AVI_read_frame(avi_t *AVI, u8 *vidbuf, int *keyframe)
{
	int n;

	if(AVI->mode==AVI_MODE_WRITE) {
		AVI_errno = AVI_ERR_NOT_PERM;
		return -1;
	}
	if(!AVI->video_index)         {
		AVI_errno = AVI_ERR_NO_IDX;
		return -1;
	}

	if(AVI->video_pos < 0 || AVI->video_pos >= AVI->video_frames) return -1;
	n = (u32) AVI->video_index[AVI->video_pos].len;

	*keyframe = (AVI->video_index[AVI->video_pos].key==0x10) ? 1:0;

	if (vidbuf == NULL) {
		AVI->video_pos++;
		return n;
	}

	gf_fseek(AVI->fdes, AVI->video_index[AVI->video_pos].pos, SEEK_SET);

	if (avi_read(AVI->fdes,vidbuf,n) != (u32) n)
	{
		AVI_errno = AVI_ERR_READ;
		return -1;
	}

	AVI->video_pos++;

	return n;
}