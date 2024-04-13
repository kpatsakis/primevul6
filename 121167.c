const EbmlSemantic & EbmlSemanticContext::GetSemantic(size_t i) const
{
  assert(i<Size);
  if (i<Size)
    return MyTable[i];

  std::stringstream ss;
  ss << "EbmlSemanticContext::GetSemantic: programming error: index i outside of table size (" << i << " >= " << Size << ")";
  throw std::logic_error(ss.str());
}