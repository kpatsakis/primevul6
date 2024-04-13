void gf_bs_enable_emulation_byte_removal(GF_BitStream *bs, Bool do_remove)
{
	if (bs) {
		bs->remove_emul_prevention_byte = do_remove;
		bs->nb_zeros = 0;
	}
}