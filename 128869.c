libxlDomainDefNamespaceParse(xmlXPathContextPtr ctxt,
                             void **data)
{
    libxlDomainXmlNsDef *nsdata = NULL;
    g_autofree xmlNodePtr *nodes = NULL;
    ssize_t nnodes;
    size_t i;
    int ret = -1;

    if ((nnodes = virXPathNodeSet("./xen:commandline/xen:arg", ctxt, &nodes)) < 0)
        return -1;

    if (nnodes == 0)
        return 0;

    nsdata = g_new0(libxlDomainXmlNsDef, 1);
    nsdata->args = g_new0(char *, nnodes + 1);

    for (i = 0; i < nnodes; i++) {
        if (!(nsdata->args[nsdata->num_args++] = virXMLPropString(nodes[i], "value"))) {
            virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                           _("No device model command-line argument specified"));
            goto cleanup;
        }
    }

    *data = g_steal_pointer(&nsdata);
    ret = 0;

 cleanup:
    libxlDomainDefNamespaceFree(nsdata);
    return ret;
}