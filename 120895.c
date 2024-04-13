static void addLengthDistance(uivector* values, size_t length, size_t distance)
{
  /*values in encoded vector are those used by deflate:
  0-255: literal bytes
  256: end
  257-285: length/distance pair (length code, followed by extra length bits, distance code, extra distance bits)
  286-287: invalid*/

  unsigned length_code = (unsigned)searchCodeIndex(LENGTHBASE, 29, length);
  unsigned extra_length = (unsigned)(length - LENGTHBASE[length_code]);
  unsigned dist_code = (unsigned)searchCodeIndex(DISTANCEBASE, 30, distance);
  unsigned extra_distance = (unsigned)(distance - DISTANCEBASE[dist_code]);

  uivector_push_back(values, length_code + FIRST_LENGTH_CODE_INDEX);
  uivector_push_back(values, extra_length);
  uivector_push_back(values, dist_code);
  uivector_push_back(values, extra_distance);
}