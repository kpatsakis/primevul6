void ogs_sbi_header_free(ogs_sbi_header_t *h)
{
    int i;
    ogs_assert(h);

    if (h->method) ogs_free(h->method);
    if (h->service.name) ogs_free(h->service.name);
    if (h->api.version) ogs_free(h->api.version);

    for (i = 0; i < OGS_SBI_MAX_NUM_OF_RESOURCE_COMPONENT &&
                        h->resource.component[i]; i++)
        ogs_free(h->resource.component[i]);
}