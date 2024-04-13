static int acp_hw_fini(void *handle)
{
	int i, ret;
	u32 val = 0;
	u32 count = 0;
	struct device *dev;
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	/* return early if no ACP */
	if (!adev->acp.acp_genpd) {
		amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_ACP, false);
		return 0;
	}

	/* Assert Soft reset of ACP */
	val = cgs_read_register(adev->acp.cgs_device, mmACP_SOFT_RESET);

	val |= ACP_SOFT_RESET__SoftResetAud_MASK;
	cgs_write_register(adev->acp.cgs_device, mmACP_SOFT_RESET, val);

	count = ACP_SOFT_RESET_DONE_TIME_OUT_VALUE;
	while (true) {
		val = cgs_read_register(adev->acp.cgs_device, mmACP_SOFT_RESET);
		if (ACP_SOFT_RESET__SoftResetAudDone_MASK ==
		    (val & ACP_SOFT_RESET__SoftResetAudDone_MASK))
			break;
		if (--count == 0) {
			dev_err(&adev->pdev->dev, "Failed to reset ACP\n");
			return -ETIMEDOUT;
		}
		udelay(100);
	}
	/* Disable ACP clock */
	val = cgs_read_register(adev->acp.cgs_device, mmACP_CONTROL);
	val &= ~ACP_CONTROL__ClkEn_MASK;
	cgs_write_register(adev->acp.cgs_device, mmACP_CONTROL, val);

	count = ACP_CLOCK_EN_TIME_OUT_VALUE;

	while (true) {
		val = cgs_read_register(adev->acp.cgs_device, mmACP_STATUS);
		if (val & (u32) 0x1)
			break;
		if (--count == 0) {
			dev_err(&adev->pdev->dev, "Failed to reset ACP\n");
			return -ETIMEDOUT;
		}
		udelay(100);
	}

	for (i = 0; i < ACP_DEVS ; i++) {
		dev = get_mfd_cell_dev(adev->acp.acp_cell[i].name, i);
		ret = pm_genpd_remove_device(dev);
		/* If removal fails, dont giveup and try rest */
		if (ret)
			dev_err(dev, "remove dev from genpd failed\n");
	}

	mfd_remove_devices(adev->acp.parent);
	kfree(adev->acp.acp_res);
	kfree(adev->acp.acp_genpd);
	kfree(adev->acp.acp_cell);

	return 0;
}