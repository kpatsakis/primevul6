static int acp_suspend(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	/* power up on suspend */
	if (!adev->acp.acp_cell)
		amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_ACP, false);
	return 0;
}