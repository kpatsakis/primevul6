static int create_xml_escape(char *pString, unsigned char c)
{ 
  int counter = 0;

  pString[counter++] = '&';
  pString[counter++] = '#';
  if(c >= 100) {
    pString[counter++] = c / 100 + '0';
    c = c % 100;
  }
  pString[counter++] = c / 10 + '0';
  c = c % 10;

  pString[counter++] = c + '0';
  pString[counter++] = ';';
  return counter; 
}