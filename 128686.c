txtwrite_put_params(gx_device * dev, gs_param_list * plist)
{
    gx_device_txtwrite_t *tdev = (gx_device_txtwrite_t *) dev;
    int ecode = 0;
    int code;
    const char *param_name;
    gs_param_string ofs;
    bool dummy;

    switch (code = param_read_string(plist, (param_name = "OutputFile"), &ofs)) {
        case 0:
            if (dev->LockSafetyParams &&
                    bytes_compare(ofs.data, ofs.size,
                        (const byte *)tdev->fname, strlen(tdev->fname))) {
                ecode = gs_note_error(gs_error_invalidaccess);
                goto ofe;
            }
            if (ofs.size >= gp_file_name_sizeof)
                ecode = gs_error_limitcheck;
            else
                break;
            goto ofe;
        default:
            ecode = code;
          ofe:param_signal_error(plist, param_name, ecode);
        /* fall through */
        case 1:
            ofs.data = 0;
            break;
    }

    if (ecode < 0)
        return ecode;

    code = param_read_int(plist, "TextFormat", &tdev->TextFormat);
    if (code < 0)
        return code;

    code = param_read_bool(plist, "WantsToUnicode", &dummy);
    if (code < 0)
        return code;

    code = param_read_bool(plist, "HighLevelDevice", &dummy);
    if (code < 0)
        return code;

    code = param_read_bool(plist, "PreserveTrMode", &dummy);
    if (code < 0)
        return code;

    code = gx_default_put_params(dev, plist);
    if (code < 0)
        return code;

    dev->interpolate_control = 0;

    if (ofs.data != 0) {	/* Close the file if it's open. */
        if (tdev->file != 0) {
            gp_fclose(tdev->file);
            tdev->file = 0;
        }
        memcpy(tdev->fname, ofs.data, ofs.size);
        tdev->fname[ofs.size] = 0;
    }
    return 0;
}