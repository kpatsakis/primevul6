static void test_heap_overflow_vrend_renderer_transfer_write_iov()
{
   struct virgl_renderer_resource_create_args args;
   args.handle = 4;
   args.target = 0;
   args.format = 4;
   args.bind = 131072;
   args.width = 0;
   args.height = 1;
   args.depth = 1;
   args.array_size = 0;
   args.last_level = 0;
   args.nr_samples = 0;
   args.flags = 0;

   virgl_renderer_resource_create(&args, NULL, 0);
   virgl_renderer_ctx_attach_resource(ctx_id, args.handle);

   char data[16];
   memset(data, 'A', 16);
   uint32_t cmd[11 + 4 +1];

   int i = 0;
   cmd[i++] = (11+4) << 16 | 0 << 8 | VIRGL_CCMD_RESOURCE_INLINE_WRITE;
   cmd[i++] = 4; // handle
   cmd[i++] = 0; // level
   cmd[i++] = 0; // usage
   cmd[i++] = 0; // stride
   cmd[i++] = 0; // layer_stride
   cmd[i++] = 0; // x
   cmd[i++] = 0; // y
   cmd[i++] = 0; // z
   cmd[i++] = 0x80000000; // w
   cmd[i++] = 0; // h
   cmd[i++] = 0; // d
   memcpy(&cmd[i], data, 16);

   virgl_renderer_submit_cmd((void *) cmd, ctx_id, 11 + 4 + 1);
}