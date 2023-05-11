#include "expected.h"
#include <iostream>

int main()
{
  int n = 4;
  auto a = expected<int, void>(n);
  auto b = expected<int, float>(3);
  auto c = expected<void, int>(success);
  auto d = expected<void, void>(success);

  std::cout << a.and_then([](int n) { return n + 2; }).value() << std::endl;
  std::cout << b.and_then([](int n) { return n+4;}).value() << std::endl;
  std::cout << c.and_then([]() { return ;}).has_value() << std::endl;
  std::cout << d.and_then([]() { return ;}).has_value() << std::endl;

  return 0;
}
