/**
 * Copyright (C) 2022 Hao Zhang<zh970205@mail.ustc.edu.cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef VARIANT_TOOLKIT_HPP
#define VARIANT_TOOLKIT_HPP

#include <type_traits>
#include <variant>

namespace variant_toolkit {

   template<typename T>
   struct type_identity {
      using type = T;
   };

   template<typename T, template<typename...> class Primary>
   struct is_specialization_of_helper : std::false_type {};
   template<template<typename...> class Primary, typename... Args>
   struct is_specialization_of_helper<Primary<Args...>, Primary> : std::true_type {};
   template<typename T, template<typename...> class Primary>
   constexpr bool is_specialization_of = is_specialization_of_helper<T, Primary>::value;

   template<typename Variant>
   constexpr bool is_variant = is_specialization_of<Variant, std::variant>;

   template<typename T, typename Variant>
   struct is_variant_member_helper : std::false_type {};
   template<typename T, typename... Args>
   struct is_variant_member_helper<T, std::variant<Args...>> : std::disjunction<std::is_same<Args, T>...> {};
   template<typename T, typename Variant>
   constexpr bool is_variant_member = is_variant_member_helper<T, Variant>::value;

   template<typename T, typename Variant>
   struct append_to_variant_helper : type_identity<void> {};
   template<typename T, typename... Args>
   struct append_to_variant_helper<T, std::variant<Args...>> : type_identity<std::variant<Args..., T>> {};
   template<typename T, typename Variant>
   using append_to_variant = typename append_to_variant_helper<T, Variant>::type;

   template<typename T, typename Variant>
   using add_to_variant = std::conditional_t<is_variant_member<T, Variant>, Variant, append_to_variant<T, Variant>>;

   template<typename First, typename Second>
   struct combine_variant_helper : type_identity<void> {};
   template<typename First, typename Head, typename... Tail>
   struct combine_variant_helper<First, std::variant<Head, Tail...>> : combine_variant_helper<add_to_variant<Head, First>, std::variant<Tail...>> {};
   template<typename First>
   struct combine_variant_helper<First, std::variant<>> : type_identity<First> {};
   template<typename First, typename Second>
   using combine_variant = typename combine_variant_helper<First, Second>::type;

   template<typename Result, typename Variant>
   struct refine_variant_helper : type_identity<void> {};
   template<typename Result, typename Head, typename... Tail>
   struct refine_variant_helper<Result, std::variant<Head, Tail...>> :
         std::conditional<
               is_variant<Head>,
               typename refine_variant_helper<
                     combine_variant<Result, typename refine_variant_helper<std::variant<>, Head>::type>,
                     std::variant<Tail...>>::type,
               typename refine_variant_helper<add_to_variant<Head, Result>, std::variant<Tail...>>::type> {};
   template<typename Result>
   struct refine_variant_helper<Result, std::variant<>> : type_identity<Result> {};
   template<typename Variant>
   using refine_variant = typename refine_variant_helper<std::variant<std::monostate>, Variant>::type;

} // namespace variant_toolkit

#endif
