static String HHVM_FUNCTION(wddx_packet_end, const Resource& packet_id) {
  auto wddxPacket = packet_id.getTyped<WddxPacket>();
  std::string packetString = wddxPacket->packet_end();
  return String(packetString);
}