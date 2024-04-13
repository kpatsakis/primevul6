static int encode_sixpack(unsigned char *tx_buf, unsigned char *tx_buf_raw,
	int length, unsigned char tx_delay)
{
	int count = 0;
	unsigned char checksum = 0, buf[400];
	int raw_count = 0;

	tx_buf_raw[raw_count++] = SIXP_PRIO_CMD_MASK | SIXP_TX_MASK;
	tx_buf_raw[raw_count++] = SIXP_SEOF;

	buf[0] = tx_delay;
	for (count = 1; count < length; count++)
		buf[count] = tx_buf[count];

	for (count = 0; count < length; count++)
		checksum += buf[count];
	buf[length] = (unsigned char) 0xff - checksum;

	for (count = 0; count <= length; count++) {
		if ((count % 3) == 0) {
			tx_buf_raw[raw_count++] = (buf[count] & 0x3f);
			tx_buf_raw[raw_count] = ((buf[count] >> 2) & 0x30);
		} else if ((count % 3) == 1) {
			tx_buf_raw[raw_count++] |= (buf[count] & 0x0f);
			tx_buf_raw[raw_count] =	((buf[count] >> 2) & 0x3c);
		} else {
			tx_buf_raw[raw_count++] |= (buf[count] & 0x03);
			tx_buf_raw[raw_count++] = (buf[count] >> 2);
		}
	}
	if ((length % 3) != 2)
		raw_count++;
	tx_buf_raw[raw_count++] = SIXP_SEOF;
	return raw_count;
}