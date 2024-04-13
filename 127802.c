isdn_ppp_frame_log(char *info, char *data, int len, int maxlen, int unit, int slot)
{
	int cnt,
		j,
		i;
	char buf[80];

	if (len < maxlen)
		maxlen = len;

	for (i = 0, cnt = 0; cnt < maxlen; i++) {
		for (j = 0; j < 16 && cnt < maxlen; j++, cnt++)
			sprintf(buf + j * 3, "%02x ", (unsigned char)data[cnt]);
		printk(KERN_DEBUG "[%d/%d].%s[%d]: %s\n", unit, slot, info, i, buf);
	}
}