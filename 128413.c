struct sctp_association *sctp_association_new(const struct sctp_endpoint *ep,
					 const struct sock *sk,
					 sctp_scope_t scope,
					 gfp_t gfp)
{
	struct sctp_association *asoc;

	asoc = kzalloc(sizeof(*asoc), gfp);
	if (!asoc)
		goto fail;

	if (!sctp_association_init(asoc, ep, sk, scope, gfp))
		goto fail_init;

	SCTP_DBG_OBJCNT_INC(assoc);

	pr_debug("Created asoc %p\n", asoc);

	return asoc;

fail_init:
	kfree(asoc);
fail:
	return NULL;
}