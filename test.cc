/*
 * test.c
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>


struct A {
  char a;
  int b;
};
struct TestA {
  int num;
  A a[0];
};

int main() {
  std::cout << sizeof(TestA) << std::endl;
  std::cout << sizeof(int) << std::endl;
  std::cout << sizeof(A) << std::endl;
  return 0;
};

