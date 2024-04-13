safe_add (int a, int b)
{
  if (a > INT_MAX/2 || b > INT_MAX/2)
    return a > b ? a : b;
  else
    return a + b;
}