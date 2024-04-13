bool DISOpticalFlowImpl::ocl_Densification(UMat &dst_Ux, UMat &dst_Uy, UMat &src_Sx, UMat &src_Sy, UMat &_I0, UMat &_I1)
{
    size_t globalSize[] = {(size_t)w, (size_t)h};
    size_t localSize[]  = {16, 16};

    ocl::Kernel kernel("dis_densification", ocl::video::dis_flow_oclsrc);
    kernel.args(ocl::KernelArg::PtrReadOnly(src_Sx),
                ocl::KernelArg::PtrReadOnly(src_Sy),
                ocl::KernelArg::PtrReadOnly(_I0),
                ocl::KernelArg::PtrReadOnly(_I1),
                (int)patch_size, (int)patch_stride,
                (int)w, (int)h, (int)ws,
                ocl::KernelArg::PtrWriteOnly(dst_Ux),
                ocl::KernelArg::PtrWriteOnly(dst_Uy));
    return kernel.run(2, globalSize, localSize, false);
}