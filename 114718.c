void ndpi_dump_protocols(struct ndpi_detection_module_struct *ndpi_str) {
  int i;

  for (i = 0; i < (int) ndpi_str->ndpi_num_supported_protocols; i++)
    printf("%3d %-22s %-8s %-12s %s\n", i, ndpi_str->proto_defaults[i].protoName,
	   ndpi_get_l4_proto_name(ndpi_get_l4_proto_info(ndpi_str, i)),
	   ndpi_get_proto_breed_name(ndpi_str, ndpi_str->proto_defaults[i].protoBreed),
	   ndpi_category_get_name(ndpi_str, ndpi_str->proto_defaults[i].protoCategory));
}