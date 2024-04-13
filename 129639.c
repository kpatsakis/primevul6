static void initialize_environment()
{
   setenv("LIBGL_ALWAYS_SOFTWARE", "true", 0);
   setenv("GALLIUM_DRIVER", "softpipe", 0);
   test_egl = virgl_egl_init(NULL, true, true);
   assert(test_egl);

   virgl_renderer_init(&cookie, VIRGL_RENDERER_USE_GLES|
                       VIRGL_RENDERER_USE_SURFACELESS, &fuzzer_cbs);

   const char *name = "fuzzctx";
   virgl_renderer_context_create(ctx_id, (unsigned)strlen(name), name);
}