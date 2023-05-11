#pragma once

#include <type_traits>

namespace gb {
namespace detail {


template <class _Rollback>
struct exception_guard {

  exception_guard() = delete;

  constexpr explicit exception_guard(_Rollback __rollback)
      : __rollback_(std::move(__rollback)), __completed_(false) {}

  constexpr
  exception_guard(exception_guard&& __other)
      noexcept(std::is_nothrow_move_constructible_v<_Rollback>)
      : __rollback_(std::move(__other.__rollback_)), __completed_(__other.__completed_) {
    __other.__completed_ = true;
  }

  exception_guard(exception_guard const&)            = delete;
  exception_guard& operator=(exception_guard const&) = delete;
  exception_guard& operator=(exception_guard&&)      = delete;

  constexpr void __complete() noexcept { __completed_ = true; }

  constexpr ~exception_guard() {
    if (!__completed_)
      __rollback_();
  }

private:
  _Rollback __rollback_;
  bool __completed_;
};


template <class _Rollback>
constexpr exception_guard<_Rollback> make_exception_guard(_Rollback __rollback) {
  return exception_guard<_Rollback>(std::move(__rollback));
}

}

}
