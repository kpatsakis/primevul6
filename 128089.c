static int acp_resume(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	/* power down again on resume */
	if (!adev->acp.acp_cell)
		amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_ACP, true);
	return 0;
}