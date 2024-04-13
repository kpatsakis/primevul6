ndpi_port_range *ndpi_build_default_ports_range(ndpi_port_range *ports, u_int16_t portA_low, u_int16_t portA_high,
                                                u_int16_t portB_low, u_int16_t portB_high, u_int16_t portC_low,
                                                u_int16_t portC_high, u_int16_t portD_low, u_int16_t portD_high,
                                                u_int16_t portE_low, u_int16_t portE_high) {
  int i = 0;

  ports[i].port_low = portA_low, ports[i].port_high = portA_high;
  i++;
  ports[i].port_low = portB_low, ports[i].port_high = portB_high;
  i++;
  ports[i].port_low = portC_low, ports[i].port_high = portC_high;
  i++;
  ports[i].port_low = portD_low, ports[i].port_high = portD_high;
  i++;
  ports[i].port_low = portE_low, ports[i].port_high = portE_high;

  return(ports);
}