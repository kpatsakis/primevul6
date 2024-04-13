void gf_bs_reassign(GF_BitStream *bs, FILE *stream)
{
	if (!bs) return;
	switch (bs->bsmode) {
	case GF_BITSTREAM_FILE_WRITE:
	case GF_BITSTREAM_FILE_READ:
		bs->stream = stream;
		if (gf_ftell(stream) != bs->position)
			gf_bs_seek(bs, bs->position);
		break;
	}
}