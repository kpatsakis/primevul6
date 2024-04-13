static void test_format_util_format_is_rgb_nullptr_deref_trigger_illegal_resource()
{
   struct virgl_renderer_resource_create_args args;
   args.handle = 8;
   args.target = 0;
   args.format = 109;
   args.bind = 8;
   args.width = 2;
   args.height = 0;
   args.depth = 0;
   args.array_size = 0;
   args.last_level = 0;
   args.nr_samples = 0;
   args.flags = 0;

   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   uint32_t cmd[VIRGL_OBJ_SAMPLER_VIEW_SIZE + 1];

   int i = 0;
   cmd[i++] = VIRGL_OBJ_SAMPLER_VIEW_SIZE << 16 | VIRGL_OBJECT_SAMPLER_VIEW << 8 | VIRGL_CCMD_CREATE_OBJECT;
   cmd[i++] = 35; // handle
   cmd[i++] = 8; // res_handle
   cmd[i++] = 3107; //format
   cmd[i++] = 0; //first element
   cmd[i++] = 0; // last element
   cmd[i++] = 0; //swizzle

   virgl_renderer_submit_cmd((void *) cmd, ctx_id, VIRGL_OBJ_SAMPLER_VIEW_SIZE + 1);
}