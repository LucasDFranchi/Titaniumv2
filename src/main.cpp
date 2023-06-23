#include <iostream>

class Foo {
public:
  Foo(int bar) { this->bar_ = bar; };
  int getFoo(void) { return this->bar_; }

private:
  int bar_;
};

int main(void) {
  Foo f(10);
  std::cout << f.getFoo();

  return 0;
}

extern "C" void app_main(void) { main(); }