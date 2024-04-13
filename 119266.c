DISOpticalFlowImpl::Densification_ParBody::Densification_ParBody(DISOpticalFlowImpl &_dis, int _nstripes, int _h,
                                                                 Mat &dst_Ux, Mat &dst_Uy, Mat &src_Sx, Mat &src_Sy,
                                                                 Mat &_I0, Mat &_I1)
    : dis(&_dis), nstripes(_nstripes), h(_h), Ux(&dst_Ux), Uy(&dst_Uy), Sx(&src_Sx), Sy(&src_Sy), I0(&_I0), I1(&_I1)
{
    stripe_sz = (int)ceil(h / (double)nstripes);
}