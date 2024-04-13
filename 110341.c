static int amba_match(struct device *dev, struct device_driver *drv)
{
	struct amba_device *pcdev = to_amba_device(dev);
	struct amba_driver *pcdrv = to_amba_driver(drv);

	return amba_lookup(pcdrv->id_table, pcdev) != NULL;
}