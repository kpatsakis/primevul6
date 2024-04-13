static inline bool sctp_peer_needs_update(struct sctp_association *asoc)
{
	struct net *net = sock_net(asoc->base.sk);
	switch (asoc->state) {
	case SCTP_STATE_ESTABLISHED:
	case SCTP_STATE_SHUTDOWN_PENDING:
	case SCTP_STATE_SHUTDOWN_RECEIVED:
	case SCTP_STATE_SHUTDOWN_SENT:
		if ((asoc->rwnd > asoc->a_rwnd) &&
		    ((asoc->rwnd - asoc->a_rwnd) >= max_t(__u32,
			   (asoc->base.sk->sk_rcvbuf >> net->sctp.rwnd_upd_shift),
			   asoc->pathmtu)))
			return true;
		break;
	default:
		break;
	}
	return false;
}