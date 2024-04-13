cma_validate_port(struct ib_device *device, u32 port,
		  enum ib_gid_type gid_type,
		  union ib_gid *gid,
		  struct rdma_id_private *id_priv)
{
	struct rdma_dev_addr *dev_addr = &id_priv->id.route.addr.dev_addr;
	int bound_if_index = dev_addr->bound_dev_if;
	const struct ib_gid_attr *sgid_attr;
	int dev_type = dev_addr->dev_type;
	struct net_device *ndev = NULL;

	if (!rdma_dev_access_netns(device, id_priv->id.route.addr.dev_addr.net))
		return ERR_PTR(-ENODEV);

	if ((dev_type == ARPHRD_INFINIBAND) && !rdma_protocol_ib(device, port))
		return ERR_PTR(-ENODEV);

	if ((dev_type != ARPHRD_INFINIBAND) && rdma_protocol_ib(device, port))
		return ERR_PTR(-ENODEV);

	if (dev_type == ARPHRD_ETHER && rdma_protocol_roce(device, port)) {
		ndev = dev_get_by_index(dev_addr->net, bound_if_index);
		if (!ndev)
			return ERR_PTR(-ENODEV);
	} else {
		gid_type = IB_GID_TYPE_IB;
	}

	sgid_attr = rdma_find_gid_by_port(device, gid, gid_type, port, ndev);
	if (ndev)
		dev_put(ndev);
	return sgid_attr;
}