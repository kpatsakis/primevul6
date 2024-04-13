static bool r_bin_mdmp_patch_pe_headers(RBuffer *pe_buf) {
	int i;
	Pe64_image_dos_header dos_hdr;
	Pe64_image_nt_headers nt_hdr;
	Pe64_image_section_header *section_hdrs;

	r_buf_read_at (pe_buf, 0, (ut8 *)&dos_hdr, sizeof (Pe64_image_dos_header));
	r_buf_read_at (pe_buf, dos_hdr.e_lfanew, (ut8 *)&nt_hdr, sizeof (Pe64_image_nt_headers));

	/* Patch RawData in headers */
	section_hdrs = (Pe64_image_section_header *)(pe_buf->buf + dos_hdr.e_lfanew + 4 + sizeof (Pe64_image_file_header) + nt_hdr.file_header.SizeOfOptionalHeader);
	for (i = 0; i < nt_hdr.file_header.NumberOfSections; i++) {
		section_hdrs[i].PointerToRawData = section_hdrs[i].VirtualAddress;
	}

	return true;
}