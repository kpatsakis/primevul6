static void selinux_req_classify_flow(const struct request_sock *req,
				      struct flowi_common *flic)
{
	flic->flowic_secid = req->secid;
}