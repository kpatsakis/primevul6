libxlDomainDefNamespaceFree(void *nsdata)
{
    libxlDomainXmlNsDef *def = nsdata;

    if (!def)
        return;

    g_strfreev(def->args);
    g_free(def);
}