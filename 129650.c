static void test_double_free_in_vrend_renderer_blit_int_trigger_invalid_formats()
{
   struct virgl_renderer_resource_create_args args;
   args.handle = 1;
   args.target = 0;
   args.format = 262144;
   args.bind = 131072;
   args.width = 1;
   args.height = 1;
   args.depth = 1;
   args.array_size = 0;
   args.last_level = 0;
   args.nr_samples = 0;
   args.flags = 0;

   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   args.handle = 6;
   args.target = 4;
   args.format = 1;
   args.bind = 2;
   args.width = 2;
   args.height = 0;
   args.depth = 1;
   args.array_size = 6;
   args.last_level = 2;
   args.nr_samples = 0;
   args.flags = 0;

   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   args.handle = 1;
   args.target = 7;
   args.format = 237;
   args.bind = 1;
   args.width = 6;
   args.height = 0;
   args.depth = 1;
   args.array_size = 0;
   args.last_level = 0;
   args.nr_samples = 6;
   args.flags = 0;

   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   uint32_t cmd[VIRGL_CMD_BLIT_SIZE + 1];

   int i = 0;
   cmd[i++] = VIRGL_CMD_BLIT_SIZE << 16 | 0 << 8 | VIRGL_CCMD_BLIT;
   cmd[i++] = 17113104; // s0
   cmd[i++] = 1; // minxy
   cmd[i++] = 36; // maxxy
   cmd[i++] = 6; //dhandle
   cmd[i++] = 0; // dlevel
   cmd[i++] = 0; //dformat
   cmd[i++] = 0; //dx
   cmd[i++] = 0; // dy
   cmd[i++] = 0; // dz
   cmd[i++] = 6; //dw
   cmd[i++] = 0; // dh
   cmd[i++] = 0; // dd
   cmd[i++] = 1; //shandle
   cmd[i++] = 0; //slevel
   cmd[i++] = 0; //sformat
   cmd[i++] = 0; //sx
   cmd[i++] = 0; // sy
   cmd[i++] = 268435456; // sz
   cmd[i++] = 0; // sw
   cmd[i++] = 0; // sh
   cmd[i++] = 0; // sd

   virgl_renderer_submit_cmd((void *) cmd, ctx_id, VIRGL_CMD_BLIT_SIZE + 1);
}