void gf_m4v_rewrite_pl(u8 **o_data, u32 *o_dataLen, u8 PL)
{
	u32 pos = 0;
	unsigned char *data = (unsigned char *)*o_data;
	u32 dataLen = *o_dataLen;

	while (pos + 4 < dataLen) {
		if (!data[pos] && !data[pos + 1] && (data[pos + 2] == 0x01) && (data[pos + 3] == M4V_VOS_START_CODE)) {
			data[pos + 4] = PL;
			return;
		}
		pos++;
	}
	/*emulate VOS at beggining*/
	(*o_data) = (char *)gf_malloc(sizeof(char)*(dataLen + 5));
	(*o_data)[0] = 0;
	(*o_data)[1] = 0;
	(*o_data)[2] = 1;
	(*o_data)[3] = (char)M4V_VOS_START_CODE;
	(*o_data)[4] = PL;
	memcpy((*o_data + 5), data, sizeof(char)*dataLen);
	gf_free(data);
	(*o_dataLen) = dataLen + 5;
}