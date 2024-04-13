update_bar_address(struct  pci_vdev *dev, uint64_t addr, int idx, int type)
{
	int decode;

	if (dev->bar[idx].type == PCIBAR_IO)
		decode = porten(dev);
	else
		decode = memen(dev);

	if (decode)
		unregister_bar(dev, idx);

	switch (type) {
	case PCIBAR_IO:
	case PCIBAR_MEM32:
		dev->bar[idx].addr = addr;
		break;
	case PCIBAR_MEM64:
		dev->bar[idx].addr &= ~0xffffffffUL;
		dev->bar[idx].addr |= addr;
		break;
	case PCIBAR_MEMHI64:
		dev->bar[idx].addr &= 0xffffffff;
		dev->bar[idx].addr |= addr;
		break;
	default:
		assert(0);
	}

	if (decode)
		register_bar(dev, idx);
}