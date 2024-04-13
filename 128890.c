libxlDomainDefNamespaceFormatXML(virBuffer *buf,
                                 void *nsdata)
{
    libxlDomainXmlNsDef *cmd = nsdata;
    size_t i;

    if (!cmd->num_args)
        return 0;

    virBufferAddLit(buf, "<xen:commandline>\n");
    virBufferAdjustIndent(buf, 2);

    for (i = 0; i < cmd->num_args; i++)
        virBufferEscapeString(buf, "<xen:arg value='%s'/>\n",
                              cmd->args[i]);

    virBufferAdjustIndent(buf, -2);
    virBufferAddLit(buf, "</xen:commandline>\n");

    return 0;
}