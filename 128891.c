libxlDomainObjPrivateXMLFormat(virBuffer *buf,
                               virDomainObj *vm)
{
    libxlDomainObjPrivate *priv = vm->privateData;

    if (priv->lockState)
        virBufferAsprintf(buf, "<lockstate>%s</lockstate>\n", priv->lockState);

    return 0;
}