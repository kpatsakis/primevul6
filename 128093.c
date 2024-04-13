 * I've copied my routines from gdevcdj.c
 */

static int
lxm_get_params(gx_device *pdev, gs_param_list *plist)
{
    lxm_device* const ldev = (lxm_device*)pdev;
    int code = gdev_prn_get_params(pdev, plist);

    if ( code < 0 ) return code;
    code = param_write_int(plist,
                           "HeadSeparation",