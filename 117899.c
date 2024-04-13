ModuleExport void UnregisterJPEGImage(void)
{
  (void) UnregisterMagickInfo("PJPG");
  (void) UnregisterMagickInfo("JPS");
  (void) UnregisterMagickInfo("JPG");
  (void) UnregisterMagickInfo("JPEG");
  (void) UnregisterMagickInfo("JPE");
}