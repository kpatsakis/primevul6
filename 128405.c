static u8 sctp_trans_score(const struct sctp_transport *trans)
{
	return sctp_trans_state_to_prio_map[trans->state];
}