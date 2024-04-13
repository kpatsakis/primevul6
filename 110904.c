static void vvc_compute_poc(VVCSliceInfo *si)
{
	u32 max_poc_lsb = 1 << (si->sps->log2_max_poc_lsb);

	if (si->poc_msb_cycle_present_flag) {
		si->poc_msb = si->poc_msb_cycle;
	} else {
		if ((si->poc_lsb < si->poc_lsb_prev) && (si->poc_lsb_prev - si->poc_lsb >= max_poc_lsb / 2))
			si->poc_msb = si->poc_msb_prev + max_poc_lsb;
		else if ((si->poc_lsb > si->poc_lsb_prev) && (si->poc_lsb - si->poc_lsb_prev > max_poc_lsb / 2))
			si->poc_msb = si->poc_msb_prev - max_poc_lsb;
		else
			si->poc_msb = si->poc_msb_prev;
	}

	si->poc = si->poc_msb + si->poc_lsb;
}