static int cma_ib_check_req_qp_type(const struct rdma_cm_id *id,
				    const struct ib_cm_event *ib_event)
{
	return (((ib_event->event == IB_CM_REQ_RECEIVED) &&
		 (ib_event->param.req_rcvd.qp_type == id->qp_type)) ||
		((ib_event->event == IB_CM_SIDR_REQ_RECEIVED) &&
		 (id->qp_type == IB_QPT_UD)) ||
		(!id->qp_type));
}