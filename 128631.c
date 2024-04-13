txtwrite_dev_spec_op(gx_device *pdev, int dev_spec_op, void *data, int size)
{
    switch (dev_spec_op) {
        case gxdso_get_dev_param:
            {
                int code;
                dev_param_req_t *request = (dev_param_req_t *)data;
                code = txtwrite_get_param(pdev, request->Param, request->list);
                if (code != gs_error_undefined)
                    return code;
            }
    }
    return gx_default_dev_spec_op(pdev, dev_spec_op, data, size);
}