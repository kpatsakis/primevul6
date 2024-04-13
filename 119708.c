void CLASS Kodak_WB_0x08tags(int wb, unsigned type)
{
  float mul[3] = {1, 1, 1}, num, mul2;
  int c;
  FORC3 mul[c] = (num = getreal(type)) == 0 ? 1 : num;
  imgdata.color.WB_Coeffs[wb][1] = imgdata.color.WB_Coeffs[wb][3] = mul[1];
  mul2 = mul[1] * mul[1];
  imgdata.color.WB_Coeffs[wb][0] = mul2 / mul[0];
  imgdata.color.WB_Coeffs[wb][2] = mul2 / mul[2];
  return;
}