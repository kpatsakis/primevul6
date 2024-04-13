bool DISOpticalFlowImpl::ocl_precomputeStructureTensor(UMat &dst_I0xx, UMat &dst_I0yy, UMat &dst_I0xy,
                                                       UMat &dst_I0x, UMat &dst_I0y, UMat &I0x, UMat &I0y)
{
    size_t globalSizeX[] = {(size_t)h};
    size_t localSizeX[]  = {16};

    ocl::Kernel kernelX("dis_precomputeStructureTensor_hor", ocl::video::dis_flow_oclsrc);
    kernelX.args(ocl::KernelArg::PtrReadOnly(I0x),
                 ocl::KernelArg::PtrReadOnly(I0y),
                 (int)patch_size, (int)patch_stride,
                 (int)w, (int)h, (int)ws,
                 ocl::KernelArg::PtrWriteOnly(u_I0xx_buf_aux),
                 ocl::KernelArg::PtrWriteOnly(u_I0yy_buf_aux),
                 ocl::KernelArg::PtrWriteOnly(u_I0xy_buf_aux),
                 ocl::KernelArg::PtrWriteOnly(u_I0x_buf_aux),
                 ocl::KernelArg::PtrWriteOnly(u_I0y_buf_aux));
    if (!kernelX.run(1, globalSizeX, localSizeX, false))
        return false;

    size_t globalSizeY[] = {(size_t)ws};
    size_t localSizeY[]  = {16};

    ocl::Kernel kernelY("dis_precomputeStructureTensor_ver", ocl::video::dis_flow_oclsrc);
    kernelY.args(ocl::KernelArg::PtrReadOnly(u_I0xx_buf_aux),
                 ocl::KernelArg::PtrReadOnly(u_I0yy_buf_aux),
                 ocl::KernelArg::PtrReadOnly(u_I0xy_buf_aux),
                 ocl::KernelArg::PtrReadOnly(u_I0x_buf_aux),
                 ocl::KernelArg::PtrReadOnly(u_I0y_buf_aux),
                 (int)patch_size, (int)patch_stride,
                 (int)w, (int)h, (int)ws,
                 ocl::KernelArg::PtrWriteOnly(dst_I0xx),
                 ocl::KernelArg::PtrWriteOnly(dst_I0yy),
                 ocl::KernelArg::PtrWriteOnly(dst_I0xy),
                 ocl::KernelArg::PtrWriteOnly(dst_I0x),
                 ocl::KernelArg::PtrWriteOnly(dst_I0y));
    return kernelY.run(1, globalSizeY, localSizeY, false);
}