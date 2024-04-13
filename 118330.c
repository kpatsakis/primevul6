static ut64 r_buf_read64le (RBuffer *buf, ut64 off) {
	ut8 data[8] = {0};
	r_buf_read_at (buf, off, data, 8);
	return r_read_le64 (data);
}