roce_get_net_dev_by_cm_event(const struct ib_cm_event *ib_event)
{
	const struct ib_gid_attr *sgid_attr = NULL;
	struct net_device *ndev;

	if (ib_event->event == IB_CM_REQ_RECEIVED)
		sgid_attr = ib_event->param.req_rcvd.ppath_sgid_attr;
	else if (ib_event->event == IB_CM_SIDR_REQ_RECEIVED)
		sgid_attr = ib_event->param.sidr_req_rcvd.sgid_attr;

	if (!sgid_attr)
		return NULL;

	rcu_read_lock();
	ndev = rdma_read_gid_attr_ndev_rcu(sgid_attr);
	if (IS_ERR(ndev))
		ndev = NULL;
	else
		dev_hold(ndev);
	rcu_read_unlock();
	return ndev;
}