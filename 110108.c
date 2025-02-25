static struct domain_device *sas_ex_discover_expander(
	struct domain_device *parent, int phy_id)
{
	struct sas_expander_device *parent_ex = rphy_to_expander_device(parent->rphy);
	struct ex_phy *phy = &parent->ex_dev.ex_phy[phy_id];
	struct domain_device *child = NULL;
	struct sas_rphy *rphy;
	struct sas_expander_device *edev;
	struct asd_sas_port *port;
	int res;

	if (phy->routing_attr == DIRECT_ROUTING) {
		pr_warn("ex %016llx:%02d:D <--> ex %016llx:0x%x is not allowed\n",
			SAS_ADDR(parent->sas_addr), phy_id,
			SAS_ADDR(phy->attached_sas_addr),
			phy->attached_phy_id);
		return NULL;
	}
	child = sas_alloc_device();
	if (!child)
		return NULL;

	phy->port = sas_port_alloc(&parent->rphy->dev, phy_id);
	/* FIXME: better error handling */
	BUG_ON(sas_port_add(phy->port) != 0);


	switch (phy->attached_dev_type) {
	case SAS_EDGE_EXPANDER_DEVICE:
		rphy = sas_expander_alloc(phy->port,
					  SAS_EDGE_EXPANDER_DEVICE);
		break;
	case SAS_FANOUT_EXPANDER_DEVICE:
		rphy = sas_expander_alloc(phy->port,
					  SAS_FANOUT_EXPANDER_DEVICE);
		break;
	default:
		rphy = NULL;	/* shut gcc up */
		BUG();
	}
	port = parent->port;
	child->rphy = rphy;
	get_device(&rphy->dev);
	edev = rphy_to_expander_device(rphy);
	child->dev_type = phy->attached_dev_type;
	kref_get(&parent->kref);
	child->parent = parent;
	child->port = port;
	child->iproto = phy->attached_iproto;
	child->tproto = phy->attached_tproto;
	memcpy(child->sas_addr, phy->attached_sas_addr, SAS_ADDR_SIZE);
	sas_hash_addr(child->hashed_sas_addr, child->sas_addr);
	sas_ex_get_linkrate(parent, child, phy);
	edev->level = parent_ex->level + 1;
	parent->port->disc.max_level = max(parent->port->disc.max_level,
					   edev->level);
	sas_init_dev(child);
	sas_fill_in_rphy(child, rphy);
	sas_rphy_add(rphy);

	spin_lock_irq(&parent->port->dev_list_lock);
	list_add_tail(&child->dev_list_node, &parent->port->dev_list);
	spin_unlock_irq(&parent->port->dev_list_lock);

	res = sas_discover_expander(child);
	if (res) {
		sas_rphy_delete(rphy);
		spin_lock_irq(&parent->port->dev_list_lock);
		list_del(&child->dev_list_node);
		spin_unlock_irq(&parent->port->dev_list_lock);
		sas_put_device(child);
		return NULL;
	}
	list_add_tail(&child->siblings, &parent->ex_dev.children);
	return child;
}