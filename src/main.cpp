#include "expect.h"
#include <iostream>

int main()
{
  int n = 4;
  auto a = expect<int, void>(n);
  auto b = expect<int, float>(3);
  auto c = expect<void, int>(value_v);
  auto d = expect<void, void>(value_v);

  std::cout << a.and_then([](int n) { return n + 2; }).value() << std::endl;
  std::cout << b.and_then([](int n) { return n+4;}).value() << std::endl;
  std::cout << c.and_then([]() { return ;}).has_value() << std::endl;
  std::cout << d.and_then([]() { return ;}).has_value() << std::endl;

  return 0;
}
