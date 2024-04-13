static void sctp_assoc_free_asconf_acks(struct sctp_association *asoc)
{
	struct sctp_chunk *ack;
	struct sctp_chunk *tmp;

	list_for_each_entry_safe(ack, tmp, &asoc->asconf_ack_list,
				transmitted_list) {
		list_del_init(&ack->transmitted_list);
		sctp_chunk_free(ack);
	}
}