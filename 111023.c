Bool gf_eac3_parser_bs(GF_BitStream *bs, GF_AC3Config *hdr, Bool full_parse)
{
	u32 fscod, bsid, ac3_mod, freq, framesize, syncword, substreamid, lfon, channels, numblkscod, strmtyp, frmsiz;
	u64 pos;
	u16 chanmap;
	static u32 numblks[4] = {1, 2, 3, 6};

	if (!hdr || (gf_bs_available(bs) < 6))
		return GF_FALSE;
	if (!AC3_FindSyncCodeBS(bs))
		return GF_FALSE;

	pos = gf_bs_get_position(bs);
	framesize = 0;
	numblkscod = 0;
	memset(hdr, 0, sizeof(GF_AC3Config));

block:
	syncword = gf_bs_read_u16(bs);
	if (syncword != 0x0B77) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[E-AC3] Wrong sync word detected (0x%X - expecting 0x0B77).\n", syncword));
		return GF_FALSE;
	}

	strmtyp = gf_bs_read_int_log(bs, 2, "strmtyp");
	substreamid = gf_bs_read_int_log(bs, 3, "substreamid");
	//next main (independent) AU, done with this frame
	if ((strmtyp!=0x1) && ((hdr->substreams >> substreamid) & 0x1)) {
		hdr->framesize = framesize;
		gf_bs_seek(bs, pos);
		return GF_TRUE;
	}

	frmsiz = gf_bs_read_int_log(bs, 11, "frmsiz");
	framesize += 2 * (1 + frmsiz);
	fscod = gf_bs_read_int_log(bs, 2, "fscod");
	if (fscod == 0x3) {
		fscod = gf_bs_read_int_log(bs, 2, "fscod2");
		numblkscod += 6;
	}
	else {
		numblkscod += gf_bs_read_int_log(bs, 2, "numblkscod");
	}
	assert(numblkscod <= 9);


	if ((hdr->substreams >> substreamid) & 0x1) {
		//we still have sync frames following
		if (substreamid) {
			if (gf_bs_seek(bs, pos + framesize) != GF_OK) {
				gf_bs_seek(bs, pos);
				return GF_FALSE;
			}
			if ((gf_bs_available(bs) < 6) || !AC3_FindSyncCodeBS(bs)) {
				gf_bs_seek(bs, pos);
				return GF_FALSE;
			}
			goto block;
		}
	}

	hdr->substreams |= (1 << substreamid);

	switch (fscod) {
	case 0:
		freq = 48000;
		break;
	case 1:
		freq = 44100;
		break;
	case 2:
		freq = 32000;
		break;
	default:
		return GF_FALSE;
	}

	ac3_mod = gf_bs_read_int_log(bs, 3, "ac3_mod");
	lfon = gf_bs_read_int_log(bs, 1, "lfon");
	bsid = gf_bs_read_int_log(bs, 5, "bsid");
	if (!substreamid && (bsid != 16/*E-AC3*/))
		return GF_FALSE;
	gf_bs_read_int_log(bs, 5, "dialnorm");
	if (gf_bs_read_int_log(bs, 1, "compre")) {
		gf_bs_read_int_log(bs, 8, "compr");
	}
	if (ac3_mod==0) {
		gf_bs_read_int_log(bs, 5, "dialnorm2");
		if (gf_bs_read_int_log(bs, 1, "compr2e")) {
			gf_bs_read_int_log(bs, 8, "compr2");
		}
	}
	chanmap = 0;
	if (strmtyp==0x1) {
		if (gf_bs_read_int_log(bs, 1, "chanmape")) {
			chanmap = gf_bs_read_int_log(bs, 16, "chanmap");
		}
	}

	channels = ac3_mod_to_chans[ac3_mod];
	if (lfon)
		channels += 1;

	hdr->bitrate = 0;
	hdr->sample_rate = freq;
	hdr->framesize = framesize;
	if (strmtyp != 1) {
		hdr->channels = channels;
		hdr->streams[substreamid].lfon = lfon;
		if (full_parse) {
			hdr->streams[substreamid].bsid = bsid;
			hdr->streams[substreamid].bsmod = 0;
			hdr->streams[substreamid].acmod = ac3_mod;
			hdr->streams[substreamid].fscod = fscod;
			hdr->brcode = 0;
		}
		hdr->nb_streams++;
		//not clear if this is only for the independent streams
		hdr->brcode += ((frmsiz+1) * freq) / (numblks[numblkscod]*16) / 1000;

		if (lfon)
			hdr->channels += 1;

	} else {
		hdr->streams[substreamid].nb_dep_sub = substreamid;
		hdr->streams[substreamid].chan_loc |= chanmap;
	}

	if (numblkscod < 6) { //we need 6 blocks to make a sample
		if (gf_bs_seek(bs, pos + framesize) != GF_OK) {
			gf_bs_seek(bs, pos);
			return GF_FALSE;
		}

		if ((gf_bs_available(bs) < 6) || !AC3_FindSyncCodeBS(bs))
			return GF_FALSE;
		goto block;
	}

	gf_bs_seek(bs, pos);

	return GF_TRUE;
}