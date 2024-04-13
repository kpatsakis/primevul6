fuzzer_create_gl_context(UNUSED void *cookie, UNUSED int scanout_idx,
                         struct virgl_renderer_gl_ctx_param *param)
{
   struct virgl_gl_ctx_param vparams;
   vparams.shared = false;
   vparams.major_ver = param->major_ver;
   vparams.minor_ver = param->minor_ver;
   return virgl_egl_create_context(test_egl, &vparams);
}