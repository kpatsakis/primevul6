AP_DECLARE(void) ap_clear_auth_internal(void)
{
    auth_internal_per_conf_hooks = 0;
    auth_internal_per_conf_providers = 0;
}