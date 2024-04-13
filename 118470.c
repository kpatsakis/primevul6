GF_EXPORT
GF_Err gf_m2ts_restamp(u8 *buffer, u32 size, s64 ts_shift, u8 *is_pes)
{
	u32 done = 0;
	u64 pcr_mod;
//	if (!ts_shift) return GF_OK;

	pcr_mod = 0x80000000;
	pcr_mod*=4;
	while (done + 188 <= size) {
		u8 *pesh;
		u8 *pck;
		u64 pcr_base=0, pcr_ext=0;
		u16 pid;
		u8 adaptation_field, adaptation_field_length;

		pck = (u8*) buffer+done;
		if (pck[0]!=0x47) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS Restamp] Invalid sync byte %X\n", pck[0]));
			return GF_NON_COMPLIANT_BITSTREAM;
		}
		pid = ((pck[1] & 0x1f) <<8 ) + pck[2];

		adaptation_field_length = 0;
		adaptation_field = (pck[3] >> 4) & 0x3;
		if ((adaptation_field==2) || (adaptation_field==3)) {
			adaptation_field_length = pck[4];
			if ( pck[5]&0x10 /*PCR_flag*/) {
				pcr_base = (((u64)pck[6])<<25) + (pck[7]<<17) + (pck[8]<<9) + (pck[9]<<1) + (pck[10]>>7);
				pcr_ext  = ((pck[10]&1)<<8) + pck[11];

				ADJUST_TIMESTAMP(pcr_base);

				pck[6]  = (unsigned char)(0xff&(pcr_base>>25));
				pck[7]  = (unsigned char)(0xff&(pcr_base>>17));
				pck[8]  = (unsigned char)(0xff&(pcr_base>>9));
				pck[9]  = (unsigned char)(0xff&(pcr_base>>1));
				pck[10] = (unsigned char)(((0x1&pcr_base)<<7) | 0x7e | ((0x100&pcr_ext)>>8));
				if (pcr_ext != ((pck[10]&1)<<8) + pck[11]) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS Restamp] Sanity check failed for PCR restamping\n"));
					return GF_IO_ERR;
				}
				pck[11] = (unsigned char)(0xff&pcr_ext);
			}
			/*add adaptation_field_length field*/
			adaptation_field_length++;
		}
		if (!is_pes[pid] || !(pck[1]&0x40)) {
			done+=188;
			continue;
		}

		pesh = &pck[4+adaptation_field_length];

		if ((pesh[0]==0x00) && (pesh[1]==0x00) && (pesh[2]==0x01)) {
			Bool has_pts, has_dts;
			if ((pesh[6]&0xc0)!=0x80) {
				done+=188;
				continue;
			}
			has_pts = (pesh[7]&0x80);
			has_dts = has_pts ? (pesh[7]&0x40) : 0;

			if (has_pts) {
				u64 PTS;
				if (((pesh[9]&0xe0)>>4)!=0x2) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[M2TS Restamp] PID %4d: Wrong PES header, PTS decoding: '0010' expected\n", pid));
					done+=188;
					continue;
				}

				PTS = gf_m2ts_get_pts(pesh + 9);
				ADJUST_TIMESTAMP(PTS);
				rewrite_pts_dts(pesh+9, PTS);
			}

			if (has_dts) {
				u64 DTS = gf_m2ts_get_pts(pesh + 14);
				ADJUST_TIMESTAMP(DTS);
				rewrite_pts_dts(pesh+14, DTS);
			}
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[M2TS Restamp] PID %4d: Wrong PES not beginning with start code\n", pid));
		}
		done+=188;
	}
	return GF_OK;