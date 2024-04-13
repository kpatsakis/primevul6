void DISOpticalFlowImpl::collectGarbage()
{
    I0s.clear();
    I1s.clear();
    I1s_ext.clear();
    I0xs.clear();
    I0ys.clear();
    Ux.clear();
    Uy.clear();
    U.release();
    Sx.release();
    Sy.release();
    I0xx_buf.release();
    I0yy_buf.release();
    I0xy_buf.release();
    I0xx_buf_aux.release();
    I0yy_buf_aux.release();
    I0xy_buf_aux.release();

#ifdef HAVE_OPENCL
    u_I0s.clear();
    u_I1s.clear();
    u_I1s_ext.clear();
    u_I0xs.clear();
    u_I0ys.clear();
    u_Ux.clear();
    u_Uy.clear();
    u_U.release();
    u_Sx.release();
    u_Sy.release();
    u_I0xx_buf.release();
    u_I0yy_buf.release();
    u_I0xy_buf.release();
    u_I0xx_buf_aux.release();
    u_I0yy_buf_aux.release();
    u_I0xy_buf_aux.release();
#endif

    for (int i = finest_scale; i <= coarsest_scale; i++)
        variational_refinement_processors[i]->collectGarbage();
    variational_refinement_processors.clear();
}