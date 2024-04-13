ndpi_port_range *ndpi_build_default_ports(ndpi_port_range *ports, u_int16_t portA, u_int16_t portB, u_int16_t portC,
                                          u_int16_t portD, u_int16_t portE) {
  int i = 0;

  ports[i].port_low = portA, ports[i].port_high = portA;
  i++;
  ports[i].port_low = portB, ports[i].port_high = portB;
  i++;
  ports[i].port_low = portC, ports[i].port_high = portC;
  i++;
  ports[i].port_low = portD, ports[i].port_high = portD;
  i++;
  ports[i].port_low = portE, ports[i].port_high = portE;

  return(ports);
}