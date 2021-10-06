#include <iostream>

void f(char *str, int *dd, int *mm, int *yyyy)
{
  char *p;
  
  p = str;
  *dd = 0;
  while(*p != '.' && *p) {
    *dd *= 10;
    *dd += *p - '0';
    p++;
  }
  p++;
  *mm = 0;
  while(*p != '.' && *p) {
    *mm *= 10;
    *mm += *p - '0';
    p++;
  }
  p++;
  *yyyy = 0;
  while(*p != '.' && *p) {
    *yyyy *= 10;
    *yyyy += *p - '0';
    p++;
  }
  return;
}

int main(int argc, char **argv)
{
  int dd, mm, yyyy;
  
  f(argv[1], &dd, &mm, &yyyy);
  std::cout << "dd=" << dd << " mm=" << mm << " yyyy=" << yyyy << std::endl;
  return 0;
}
