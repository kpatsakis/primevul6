
GF_Err dac3_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_AC3ConfigBox *ptr = (GF_AC3ConfigBox *)s;

	if (ptr->cfg.is_ec3) s->type = GF_ISOM_BOX_TYPE_DEC3;
	e = gf_isom_box_write_header(s, bs);
	if (ptr->cfg.is_ec3) s->type = GF_ISOM_BOX_TYPE_DAC3;
	if (e) return e;

	if (ptr->cfg.is_ec3) {
		u32 i;
		gf_bs_write_int(bs, ptr->cfg.brcode, 13);
		gf_bs_write_int(bs, ptr->cfg.nb_streams - 1, 3);
		for (i=0; i<ptr->cfg.nb_streams; i++) {
			gf_bs_write_int(bs, ptr->cfg.streams[i].fscod, 2);
			gf_bs_write_int(bs, ptr->cfg.streams[i].bsid, 5);
			gf_bs_write_int(bs, ptr->cfg.streams[i].bsmod, 5);
			gf_bs_write_int(bs, ptr->cfg.streams[i].acmod, 3);
			gf_bs_write_int(bs, ptr->cfg.streams[i].lfon, 1);
			gf_bs_write_int(bs, 0, 3);
			gf_bs_write_int(bs, ptr->cfg.streams[i].nb_dep_sub, 4);
			if (ptr->cfg.streams[i].nb_dep_sub) {
				gf_bs_write_int(bs, ptr->cfg.streams[i].chan_loc, 9);
			} else {
				gf_bs_write_int(bs, 0, 1);
			}
		}
	} else {
		gf_bs_write_int(bs, ptr->cfg.streams[0].fscod, 2);
		gf_bs_write_int(bs, ptr->cfg.streams[0].bsid, 5);
		gf_bs_write_int(bs, ptr->cfg.streams[0].bsmod, 3);
		gf_bs_write_int(bs, ptr->cfg.streams[0].acmod, 3);
		gf_bs_write_int(bs, ptr->cfg.streams[0].lfon, 1);
		gf_bs_write_int(bs, ptr->cfg.brcode, 5);
		gf_bs_write_int(bs, 0, 5);
	}
	return GF_OK;