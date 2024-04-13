sixpack_decode(struct sixpack *sp, const unsigned char *pre_rbuff, int count)
{
	unsigned char inbyte;
	int count1;

	for (count1 = 0; count1 < count; count1++) {
		inbyte = pre_rbuff[count1];
		if (inbyte == SIXP_FOUND_TNC) {
			tnc_set_sync_state(sp, TNC_IN_SYNC);
			del_timer(&sp->resync_t);
		}
		if ((inbyte & SIXP_PRIO_CMD_MASK) != 0)
			decode_prio_command(sp, inbyte);
		else if ((inbyte & SIXP_STD_CMD_MASK) != 0)
			decode_std_command(sp, inbyte);
		else if ((sp->status & SIXP_RX_DCD_MASK) == SIXP_RX_DCD_MASK)
			decode_data(sp, inbyte);
	}
}