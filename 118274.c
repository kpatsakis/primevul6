static void cma_release_port(struct rdma_id_private *id_priv)
{
	struct rdma_bind_list *bind_list = id_priv->bind_list;
	struct net *net = id_priv->id.route.addr.dev_addr.net;

	if (!bind_list)
		return;

	mutex_lock(&lock);
	hlist_del(&id_priv->node);
	if (hlist_empty(&bind_list->owners)) {
		cma_ps_remove(net, bind_list->ps, bind_list->port);
		kfree(bind_list);
	}
	mutex_unlock(&lock);
}