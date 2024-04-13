static void fuzzer_destory_gl_context(UNUSED void *cookie, virgl_renderer_gl_context ctx)
{
   virgl_egl_destroy_context(test_egl, ctx);
}