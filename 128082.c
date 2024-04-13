static struct device *get_mfd_cell_dev(const char *device_name, int r)
{
	char auto_dev_name[25];
	struct device *dev;

	snprintf(auto_dev_name, sizeof(auto_dev_name),
		 "%s.%d.auto", device_name, r);
	dev = bus_find_device_by_name(&platform_bus_type, NULL, auto_dev_name);
	dev_info(dev, "device %s added to pm domain\n", auto_dev_name);

	return dev;
}