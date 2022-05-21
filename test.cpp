#include <bits/utility.h>
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

   std::cout << name(sequence_to_tuple_of_constant<std::index_sequence<1, 2, 3, 4>>) << "\n";
   auto add = [](auto x, auto y) {
      return x + y;
   };
   std::cout << name(visit_single_result<decltype(add), std::index_sequence<0, 0>, std::variant<int, double>, std::variant<long, float>>) << "\n";
   std::cout << name(visit_single_result<decltype(add), std::index_sequence<0, 1>, std::variant<int, double>, std::variant<long, float>>) << "\n";
   std::cout << name(visit_single_result<decltype(add), std::index_sequence<1, 0>, std::variant<int, double>, std::variant<long, float>>) << "\n";
   std::cout << name(visit_single_result<decltype(add), std::index_sequence<1, 1>, std::variant<int, double>, std::variant<long, float>>) << "\n";
   std::cout << name(tupled_visit_multiple_result<
                      decltype(add),
                      std::tuple<std::variant<int, double>, std::variant<long, float>>,
                      std::index_sequence<0, 0>,
                      std::index_sequence<0, 1>,
                      std::index_sequence<1, 0>,
                      std::index_sequence<1, 1>>)
             << "\n";
}
