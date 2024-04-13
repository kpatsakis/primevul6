/* Create sysfs entries for network device. */
int netdev_register_kobject(struct net_device *ndev)
{
	struct device *dev = &ndev->dev;
	const struct attribute_group **groups = ndev->sysfs_groups;
	int error = 0;

	device_initialize(dev);
	dev->class = &net_class;
	dev->platform_data = ndev;
	dev->groups = groups;

	dev_set_name(dev, "%s", ndev->name);

#ifdef CONFIG_SYSFS
	/* Allow for a device specific group */
	if (*groups)
		groups++;

	*groups++ = &netstat_group;

#if IS_ENABLED(CONFIG_WIRELESS_EXT) || IS_ENABLED(CONFIG_CFG80211)
	if (ndev->ieee80211_ptr)
		*groups++ = &wireless_group;
#if IS_ENABLED(CONFIG_WIRELESS_EXT)
	else if (ndev->wireless_handlers)
		*groups++ = &wireless_group;
#endif
#endif
#endif /* CONFIG_SYSFS */

	error = device_add(dev);
	if (error)
		return error;

	error = register_queue_kobjects(ndev);
	if (error) {
		device_del(dev);
		return error;
	}

	pm_runtime_set_memalloc_noio(dev, true);

	return error;