DISOpticalFlowImpl::PatchInverseSearch_ParBody::PatchInverseSearch_ParBody(DISOpticalFlowImpl &_dis, int _nstripes,
                                                                           int _hs, Mat &dst_Sx, Mat &dst_Sy,
                                                                           Mat &src_Ux, Mat &src_Uy, Mat &_I0, Mat &_I1,
                                                                           Mat &_I0x, Mat &_I0y, int _num_iter,
                                                                           int _pyr_level)
    : dis(&_dis), nstripes(_nstripes), hs(_hs), Sx(&dst_Sx), Sy(&dst_Sy), Ux(&src_Ux), Uy(&src_Uy), I0(&_I0), I1(&_I1),
      I0x(&_I0x), I0y(&_I0y), num_iter(_num_iter), pyr_level(_pyr_level)
{
    stripe_sz = (int)ceil(hs / (double)nstripes);
}