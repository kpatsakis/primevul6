static void add_coins(Coin* c1, const Coin* c2)
{
  size_t i;
  for(i = 0; i < c2->symbols.size; i++) uivector_push_back(&c1->symbols, c2->symbols.data[i]);
  c1->weight += c2->weight;
}