static int fuzzer_make_current(UNUSED void *cookie, UNUSED int scanout_idx,
                               virgl_renderer_gl_context ctx)
{
   return virgl_egl_make_context_current(test_egl, ctx);
}