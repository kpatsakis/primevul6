static int txtwrite_get_param(gx_device *dev, char *Param, void *list)
{
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) dev;
    gs_param_list * plist = (gs_param_list *)list;
    bool bool_T = true;

    if (strcmp(Param, "OutputFile") == 0) {
        gs_param_string ofns;

        ofns.data = (const byte *)tdev->fname,
        ofns.size = strlen(tdev->fname),
        ofns.persistent = false;
        return param_write_string(plist, "OutputFile", &ofns);
    }
    if (strcmp(Param, "WantsToUnicode") == 0) {
        return param_write_bool(plist, "WantsToUnicode", &bool_T);
    }
    if (strcmp(Param, "PreserveTrMode") == 0) {
        return param_write_bool(plist, "PreserveTrMode", &bool_T);
    }
    if (strcmp(Param, "HighLevelDevice") == 0) {
        return param_write_bool(plist, "HighLevelDevice", &bool_T);
    }
    return_error(gs_error_undefined);
}