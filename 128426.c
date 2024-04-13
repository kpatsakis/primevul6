void sctp_assoc_migrate(struct sctp_association *assoc, struct sock *newsk)
{
	struct sctp_sock *newsp = sctp_sk(newsk);
	struct sock *oldsk = assoc->base.sk;

	/* Delete the association from the old endpoint's list of
	 * associations.
	 */
	list_del_init(&assoc->asocs);

	/* Decrement the backlog value for a TCP-style socket. */
	if (sctp_style(oldsk, TCP))
		oldsk->sk_ack_backlog--;

	/* Release references to the old endpoint and the sock.  */
	sctp_endpoint_put(assoc->ep);
	sock_put(assoc->base.sk);

	/* Get a reference to the new endpoint.  */
	assoc->ep = newsp->ep;
	sctp_endpoint_hold(assoc->ep);

	/* Get a reference to the new sock.  */
	assoc->base.sk = newsk;
	sock_hold(assoc->base.sk);

	/* Add the association to the new endpoint's list of associations.  */
	sctp_endpoint_add_asoc(newsp->ep, assoc);
}