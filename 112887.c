static String HHVM_FUNCTION(wddx_serialize_value, const Variant& var,
                            const Variant& comment) {
  WddxPacket* wddxPacket = newres<WddxPacket>(comment, false, false);
  wddxPacket->serialize_value(var);
  const std::string packetString = wddxPacket->packet_end();
  return String(packetString);
}