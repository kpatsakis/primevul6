MagickExport void SetMagickMemoryMethods(
  AcquireMemoryHandler acquire_memory_handler,
  ResizeMemoryHandler resize_memory_handler,
  DestroyMemoryHandler destroy_memory_handler)
{
  /*
    Set memory methods.
  */
  if (acquire_memory_handler != (AcquireMemoryHandler) NULL)
    memory_methods.acquire_memory_handler=acquire_memory_handler;
  if (resize_memory_handler != (ResizeMemoryHandler) NULL)
    memory_methods.resize_memory_handler=resize_memory_handler;
  if (destroy_memory_handler != (DestroyMemoryHandler) NULL)
    memory_methods.destroy_memory_handler=destroy_memory_handler;
}