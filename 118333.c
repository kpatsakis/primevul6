static char *r_buf_read_string (RBuffer *buf, ut64 addr, int len) {
	ut8 *data = malloc (len);
	if (data) {
		r_buf_read_at (buf, addr, data, len);
		data[len-1] = 0;
		return data;
	}
	return NULL;
}