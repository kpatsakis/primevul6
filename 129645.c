static void test_blit_info_format_check()
{
   struct virgl_renderer_resource_create_args args;
   args.handle = 10;
   args.target = 3;
   args.format = 10;
   args.bind = 10;
   args.width = 2;
   args.height = 1;
   args.depth = 1;
   args.array_size = 0;
   args.last_level = 0;
   args.nr_samples = 0;
   args.flags = 0;

   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   uint32_t cmd[VIRGL_CMD_BLIT_SIZE + 1];

   int i = 0;
   cmd[i++] = VIRGL_CMD_BLIT_SIZE << 16 | 0 << 8 | VIRGL_CCMD_BLIT;
   cmd[i++] = 0x8000001; // s0
   cmd[i++] = 0; // minxy
   cmd[i++] = 0; // maxxy
   cmd[i++] = 10; //dhandle
   cmd[i++] = 0; // dlevel
   cmd[i++] = 0x1000029; //dformat
   cmd[i++] = 0; //dx
   cmd[i++] = 0; // dy
   cmd[i++] = 0; // dz
   cmd[i++] = 0; //dw
   cmd[i++] = 0; // dh
   cmd[i++] = 0; // dd
   cmd[i++] = 10; //shandle
   cmd[i++] = 0; //slevel
   cmd[i++] = 10; //sformat
   cmd[i++] = 0; //sx
   cmd[i++] = 0; // sy
   cmd[i++] = 0; // sz
   cmd[i++] = 0; // sw
   cmd[i++] = 0; // sh
   cmd[i++] = 0; // sd

   virgl_renderer_submit_cmd((void *) cmd, ctx_id, VIRGL_CMD_BLIT_SIZE + 1);
}