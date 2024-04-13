void gf_bs_set_eos_callback(GF_BitStream *bs, void (*EndOfStream)(void *par), void *par)
{
	bs->EndOfStream = EndOfStream;
	bs->par = par;
}