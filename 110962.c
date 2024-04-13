u32 gf_vorbis_check_frame(GF_VorbisParser *vp, u8 *data, u32 data_length)
{
	s32 block_size;
	oggpack_buffer opb;
	if (!vp) return 0;
	oggpack_readinit(&opb, (unsigned char*)data, data_length);
	/*not audio*/
	if (oggpack_read(&opb, 1) != 0) return 0;
	block_size = oggpack_read(&opb, vp->modebits);
	if (block_size == -1) return 0;
	return ((vp->mode_flag[block_size]) ? vp->max_block : vp->min_block) / (2);
}