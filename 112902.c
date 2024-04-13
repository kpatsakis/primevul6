static Resource HHVM_FUNCTION(wddx_packet_start, const Variant& comment) {
  auto wddxPacket = newres<WddxPacket>(comment, true, false);
  return Resource(wddxPacket);
}