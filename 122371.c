ModuleExport void UnregisterSIXELImage(void)
{
  (void) UnregisterMagickInfo("SIXEL");
  (void) UnregisterMagickInfo("SIX");
}