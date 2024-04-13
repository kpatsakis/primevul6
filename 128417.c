static struct sctp_transport *sctp_trans_elect_tie(struct sctp_transport *trans1,
						   struct sctp_transport *trans2)
{
	if (trans1->error_count > trans2->error_count) {
		return trans2;
	} else if (trans1->error_count == trans2->error_count &&
		   ktime_after(trans2->last_time_heard,
			       trans1->last_time_heard)) {
		return trans2;
	} else {
		return trans1;
	}
}