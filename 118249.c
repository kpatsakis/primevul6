__rdma_create_id(struct net *net, rdma_cm_event_handler event_handler,
		 void *context, enum rdma_ucm_port_space ps,
		 enum ib_qp_type qp_type, const struct rdma_id_private *parent)
{
	struct rdma_id_private *id_priv;

	id_priv = kzalloc(sizeof *id_priv, GFP_KERNEL);
	if (!id_priv)
		return ERR_PTR(-ENOMEM);

	id_priv->state = RDMA_CM_IDLE;
	id_priv->id.context = context;
	id_priv->id.event_handler = event_handler;
	id_priv->id.ps = ps;
	id_priv->id.qp_type = qp_type;
	id_priv->tos_set = false;
	id_priv->timeout_set = false;
	id_priv->min_rnr_timer_set = false;
	id_priv->gid_type = IB_GID_TYPE_IB;
	spin_lock_init(&id_priv->lock);
	mutex_init(&id_priv->qp_mutex);
	init_completion(&id_priv->comp);
	refcount_set(&id_priv->refcount, 1);
	mutex_init(&id_priv->handler_mutex);
	INIT_LIST_HEAD(&id_priv->listen_list);
	INIT_LIST_HEAD(&id_priv->mc_list);
	get_random_bytes(&id_priv->seq_num, sizeof id_priv->seq_num);
	id_priv->id.route.addr.dev_addr.net = get_net(net);
	id_priv->seq_num &= 0x00ffffff;

	rdma_restrack_new(&id_priv->res, RDMA_RESTRACK_CM_ID);
	if (parent)
		rdma_restrack_parent_name(&id_priv->res, &parent->res);

	return id_priv;
}