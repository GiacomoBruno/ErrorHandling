#include "expected.h"
#include <iostream>

int main()
{
  auto a =  gb::optional<int>(3);

  std::cout << a.value() << std::endl;

  return 0;
}
