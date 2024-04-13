static int coin_compare(const void* a, const void* b) {
  float wa = ((const Coin*)a)->weight;
  float wb = ((const Coin*)b)->weight;
  return wa > wb ? 1 : wa < wb ? -1 : 0;
}