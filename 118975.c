AP_DECLARE(apr_status_t) ap_register_auth_provider(apr_pool_t *pool,
                                                   const char *provider_group,
                                                   const char *provider_name,
                                                   const char *provider_version,
                                                   const void *provider,
                                                   int type)
{
    if ((type & AP_AUTH_INTERNAL_MASK) == AP_AUTH_INTERNAL_PER_CONF) {
        ++auth_internal_per_conf_providers;
    }

    return ap_register_provider(pool, provider_group, provider_name,
                                provider_version, provider);
}