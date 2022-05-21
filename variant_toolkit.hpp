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

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
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

   template<std::size_t Index, typename T>
   struct tuple_parameter_helper;
   template<std::size_t Index, typename Head, typename... Tail>
   struct tuple_parameter_helper<Index, std::tuple<Head, Tail...>> : tuple_parameter_helper<Index - 1, std::tuple<Tail...>> {};
   template<typename Head, typename... Tail>
   struct tuple_parameter_helper<0, std::tuple<Head, Tail...>> : type_identity<Head> {};
   template<std::size_t Index, typename T>
   struct template_parameter_helper;
   template<std::size_t Index, template<typename...> class Template, typename... Args>
   struct template_parameter_helper<Index, Template<Args...>> : tuple_parameter_helper<Index, std::tuple<Args...>> {};
   template<std::size_t Index, typename T>
   using template_parameter = typename template_parameter_helper<Index, T>::type;

   template<typename Sequence>
   struct sequence_to_tuple_of_constant_helper : type_identity<std::tuple<>> {};
   template<typename Type, Type... Value>
   struct sequence_to_tuple_of_constant_helper<std::integer_sequence<Type, Value...>> :
         type_identity<std::tuple<std::integral_constant<Type, Value>...>> {};
   template<typename Sequence>
   using sequence_to_tuple_of_constant = typename sequence_to_tuple_of_constant_helper<Sequence>::type;

   template<typename Visitor, typename Indices, typename Variants, std::size_t... IndexSequence>
   using visit_single_result_helper = std::invoke_result_t<
         Visitor,
         std::variant_alternative_t<template_parameter<IndexSequence, Indices>::value, template_parameter<IndexSequence, Variants>>...>;
   template<typename Visitor, typename Indices, typename Variants, typename Sequence>
   struct visit_single_result_passer : type_identity<void> {};
   template<typename Visitor, typename Indices, typename Variants, std::size_t... IndexSequence>
   struct visit_single_result_passer<Visitor, Indices, Variants, std::integer_sequence<std::size_t, IndexSequence...>> :
         type_identity<visit_single_result_helper<Visitor, Indices, Variants, IndexSequence...>> {};
   template<typename Visitor, typename Indices, typename Variants>
   using tupled_visit_single_result = typename visit_single_result_passer<
         Visitor,
         sequence_to_tuple_of_constant<Indices>,
         Variants,
         std::make_index_sequence<std::tuple_size_v<Variants>>>::type;
   template<typename Visitor, typename Indices, typename... Variants>
   using visit_single_result = tupled_visit_single_result<Visitor, Indices, std::tuple<Variants...>>;

   template<typename Visitor, typename Variants, typename... Indices>
   using tupled_visit_multiple_result = refine_variant<std::variant<tupled_visit_single_result<Visitor, Indices, Variants>...>>;

} // namespace variant_toolkit

#endif
