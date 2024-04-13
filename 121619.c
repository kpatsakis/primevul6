memen(struct pci_vdev *dev)
{
	uint16_t cmd;

	cmd = pci_get_cfgdata16(dev, PCIR_COMMAND);

	return (cmd & PCIM_CMD_MEMEN);
}