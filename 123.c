bool CellularNetwork::is_gsm() const {
  return network_technology_ != NETWORK_TECHNOLOGY_EVDO &&
      network_technology_ != NETWORK_TECHNOLOGY_1XRTT &&
      network_technology_ != NETWORK_TECHNOLOGY_UNKNOWN;
}
