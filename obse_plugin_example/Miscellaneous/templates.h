#pragma once

namespace cobb {
   //
   // function_traits
   // Source: <https://functionalcpp.wordpress.com/2013/08/05/function-traits/>
   //
   // Some modifications made in order to properly support x86 thiscall.
   //
   template<class F> struct function_traits;

   // function pointer
   template<class R, class... Args>
   struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)> {};

   template<class R, class... Args>
   struct function_traits<R(Args...)> {
      using return_type = R;

      static constexpr std::size_t arity = sizeof...(Args);

      template <std::size_t N> struct argument {
         static_assert(N < arity, "error: invalid parameter index.");
         using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
      };
   };
   #ifdef _WIN64
      //
      // x64 uses fastcall, not thiscall, so (this) is another argument.

      // member function pointer
      template<class C, class R, class... Args>
      struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&, Args...)> {};

      // const member function pointer
      template<class C, class R, class... Args>
      struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&, Args...)> {};
   #else
      // member function pointer
      template<class C, class R, class... Args>
      struct function_traits<R(C::*)(Args...)> : public function_traits<R(Args...)> {};

      // const member function pointer
      template<class C, class R, class... Args>
      struct function_traits<R(C::*)(Args...) const> : public function_traits<R(Args...)> {};
   #endif

   // member object pointer
   template<class C, class R>
   struct function_traits<R(C::*)> : public function_traits<R(C&)> {};
};