ModuleExport void UnregisterPSDImage(void)
{
  (void) UnregisterMagickInfo("PSB");
  (void) UnregisterMagickInfo("PSD");
}