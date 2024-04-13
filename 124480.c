static bool r_bin_mdmp_init_hdr(struct r_bin_mdmp_obj *obj) {
	obj->hdr = (struct minidump_header *)obj->b->buf;

	if (obj->hdr->number_of_streams == 0) {
		eprintf ("[WARN] No streams present!\n");
		return false;
	}

	if (obj->hdr->stream_directory_rva < sizeof (struct minidump_header))
	{
		eprintf ("[ERROR] RVA for directory resides in the header!\n");
		return false;
	}

	if (obj->hdr->check_sum) {
		eprintf ("[INFO] Checksum present but needs validating!\n");
		return false;
	}

	sdb_num_set (obj->kv, "mdmp.hdr.time_date_stamp", obj->hdr->time_date_stamp, 0);
	sdb_num_set (obj->kv, "mdmp.hdr.flags", obj->hdr->flags, 0);
	sdb_num_set (obj->kv, "mdmp_header.offset", 0, 0);
	sdb_set (obj->kv, "mdmp_header.format", "[4]zddddt[8]B Signature "
		"Version NumberOfStreams StreamDirectoryRVA CheckSum "
		"TimeDateStamp (mdmp_type)Flags", 0);

	return true;
}