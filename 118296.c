__rdma_create_kernel_id(struct net *net, rdma_cm_event_handler event_handler,
			void *context, enum rdma_ucm_port_space ps,
			enum ib_qp_type qp_type, const char *caller)
{
	struct rdma_id_private *ret;

	ret = __rdma_create_id(net, event_handler, context, ps, qp_type, NULL);
	if (IS_ERR(ret))
		return ERR_CAST(ret);

	rdma_restrack_set_name(&ret->res, caller);
	return &ret->id;
}