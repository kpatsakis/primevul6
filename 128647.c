txtwrite_get_params(gx_device * dev, gs_param_list * plist)
{
    int code;
    bool bool_T = true;
    gs_param_string ofns;
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) dev;

    code = gx_default_get_params(dev, plist);
    if (code < 0)
        return code;

    ofns.data = (const byte *)tdev->fname,
    ofns.size = strlen(tdev->fname),
    ofns.persistent = false;
    code = param_write_string(plist, "OutputFile", &ofns);
    if (code < 0)
        return code;

    code = param_write_bool(plist, "WantsToUnicode", &bool_T);
    if (code < 0)
        return code;

    code = param_write_bool(plist, "PreserveTrMode", &bool_T);
    if (code < 0)
        return code;

    code = param_write_bool(plist, "HighLevelDevice", &bool_T);
    if (code < 0)
        return code;

   code = gs_param_write_items(plist, tdev, NULL, txt_param_items);
   return code;
}