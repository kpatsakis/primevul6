static unsigned char* exif_char_dump(unsigned char * addr, int len, int offset)
{
	static unsigned char buf[4096+1];
	static unsigned char tmp[20];
	int c, i, p=0, n = 5+31;

	p += slprintf(buf+p, sizeof(buf)-p, "\nDump Len: %08X (%d)", len, len);
	if (len) {
		for(i=0; i<len+15 && p+n<=sizeof(buf); i++) {
			if (i%16==0) {
				p += slprintf(buf+p, sizeof(buf)-p, "\n%08X: ", i+offset);
			}
			if (i<len) {
				c = *addr++;
				p += slprintf(buf+p, sizeof(buf)-p, "%02X ", c);
				tmp[i%16] = c>=32 ? c : '.';
				tmp[(i%16)+1] = '\0';
			} else {
				p += slprintf(buf+p, sizeof(buf)-p, "   ");
			}
			if (i%16==15) {
				p += slprintf(buf+p, sizeof(buf)-p, "    %s", tmp);
				if (i>=len) {
					break;
				}
			}
		}
	}
	buf[sizeof(buf)-1] = '\0';
	return buf;
}