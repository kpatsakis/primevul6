 */
void netdev_unregister_kobject(struct net_device *ndev)
{
	struct device *dev = &ndev->dev;

	if (!refcount_read(&dev_net(ndev)->count))
		dev_set_uevent_suppress(dev, 1);

	kobject_get(&dev->kobj);

	remove_queue_kobjects(ndev);

	pm_runtime_set_memalloc_noio(dev, false);

	device_del(dev);