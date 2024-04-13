flatpak_context_devices_to_string (FlatpakContextDevices devices, FlatpakContextDevices valid)
{
  return flatpak_context_bitmask_to_string (devices, valid, flatpak_context_devices);
}