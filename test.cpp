#include <iostream>
#include <typeinfo>

#include <boost/core/demangle.hpp>

#include "variant_toolkit.hpp"
using namespace variant_toolkit;

#define name(...) boost::core::demangle(typeid(__VA_ARGS__).name())

int main() {
   std::cout << is_variant<int> << "\n";
   std::cout << is_variant<std::variant<int, float>> << "\n";
   std::cout << is_variant_member<int, std::variant<float>> << "\n";
   std::cout << is_variant_member<float, std::variant<double>> << "\n";
   std::cout << is_variant_member<int, std::variant<int, float>> << "\n";
   std::cout << name(add_to_variant<int, std::variant<int, float>>) << "\n";
   std::cout << name(add_to_variant<double, std::variant<int, float>>) << "\n";
   std::cout << name(combine_variant<std::variant<long, double>, std::variant<int, float>>) << "\n";
   std::cout << name(combine_variant<std::variant<int, double>, std::variant<int, float>>) << "\n";
   std::cout << name(refine_variant<std::variant<int, double, long>>) << "\n";
   std::cout << name(refine_variant<std::variant<std::variant<int, double>, std::variant<int, float>, float>>) << "\n";
}
