R_API RBinJavaInterfaceInfo *r_bin_java_read_next_interface_item(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	ut8 idx[2] = {
		0
	};
	RBinJavaInterfaceInfo *ifobj;
	const ut8 *if_buf = buf + offset;
	if (offset + 2 >= len) {
		return NULL;
	}
	memcpy (&idx, if_buf, 2);
	ifobj = r_bin_java_interface_new (bin, if_buf, len - offset);
	if (ifobj) {
		ifobj->file_offset = offset;
	}
	return ifobj;
}