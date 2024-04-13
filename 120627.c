
GF_Err dac3_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_AC3ConfigBox *ptr = (GF_AC3ConfigBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	if (ptr->cfg.is_ec3) {
		u32 i;
		ptr->cfg.brcode = gf_bs_read_int(bs, 13);
		ptr->cfg.nb_streams = gf_bs_read_int(bs, 3) + 1;
		for (i=0; i<ptr->cfg.nb_streams; i++) {
			ptr->cfg.streams[i].fscod = gf_bs_read_int(bs, 2);
			ptr->cfg.streams[i].bsid = gf_bs_read_int(bs, 5);
			ptr->cfg.streams[i].bsmod = gf_bs_read_int(bs, 5);
			ptr->cfg.streams[i].acmod = gf_bs_read_int(bs, 3);
			ptr->cfg.streams[i].lfon = gf_bs_read_int(bs, 1);
			gf_bs_read_int(bs, 3);
			ptr->cfg.streams[i].nb_dep_sub = gf_bs_read_int(bs, 4);
			if (ptr->cfg.streams[i].nb_dep_sub) {
				ptr->cfg.streams[i].chan_loc = gf_bs_read_int(bs, 9);
			} else {
				gf_bs_read_int(bs, 1);
			}
		}
	} else {
		ptr->cfg.nb_streams = 1;
		ptr->cfg.streams[0].fscod = gf_bs_read_int(bs, 2);
		ptr->cfg.streams[0].bsid = gf_bs_read_int(bs, 5);
		ptr->cfg.streams[0].bsmod = gf_bs_read_int(bs, 3);
		ptr->cfg.streams[0].acmod = gf_bs_read_int(bs, 3);
		ptr->cfg.streams[0].lfon = gf_bs_read_int(bs, 1);
		ptr->cfg.brcode = gf_bs_read_int(bs, 5);
		gf_bs_read_int(bs, 5);
	}
	return GF_OK;