static void test_format_is_has_alpha_nullptr_deref_trigger_legal_resource()
{
   struct virgl_renderer_resource_create_args args;
   args.handle = 8;
   args.target = 2;
   args.format = 10;
   args.bind = 8;
   args.width = 10;
   args.height = 45;
   args.depth = 1;
   args.array_size = 0;
   args.last_level = 0;
   args.nr_samples = 0;
   args.flags = 0;
   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   uint32_t cmd[VIRGL_OBJ_SAMPLER_VIEW_SIZE + 1];

   int i = 0;
   cmd[i++] = VIRGL_OBJ_SAMPLER_VIEW_SIZE << 16 | VIRGL_OBJECT_SAMPLER_VIEW << 8 | VIRGL_CCMD_CREATE_OBJECT;
   cmd[i++] = 35; //handle
   cmd[i++] = 8; // res_handle
   cmd[i++] = 524288; //format
   cmd[i++] = 0; //first_ele
   cmd[i++] = 0; //last_ele
   cmd[i++] = 10; //swizzle

   virgl_renderer_submit_cmd((void *) cmd, ctx_id, VIRGL_OBJ_SAMPLER_VIEW_SIZE + 1);
}