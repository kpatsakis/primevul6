static void sctp_assoc_free_asconf_queue(struct sctp_association *asoc)
{
	struct sctp_chunk *asconf;
	struct sctp_chunk *tmp;

	list_for_each_entry_safe(asconf, tmp, &asoc->addip_chunk_list, list) {
		list_del_init(&asconf->list);
		sctp_chunk_free(asconf);
	}
}