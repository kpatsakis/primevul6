static int acp_set_powergating_state(void *handle,
				     enum amd_powergating_state state)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;
	bool enable = state == AMD_PG_STATE_GATE ? true : false;

	if (adev->powerplay.pp_funcs &&
		adev->powerplay.pp_funcs->set_powergating_by_smu)
		amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_ACP, enable);

	return 0;
}