libxlDomainObjPrivateXMLParse(xmlXPathContextPtr ctxt,
                              virDomainObj *vm,
                              virDomainDefParserConfig *config G_GNUC_UNUSED)
{
    libxlDomainObjPrivate *priv = vm->privateData;

    priv->lockState = virXPathString("string(./lockstate)", ctxt);

    return 0;
}