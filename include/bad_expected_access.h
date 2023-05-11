#pragma once
#include <exception>

namespace gb {


template <class _Err>
class bad_expect_access;

template <>
class bad_expect_access<void> : public std::exception {
public:
  bad_expect_access() noexcept                           = default;
  bad_expect_access(const bad_expect_access&)            = default;
  bad_expect_access(bad_expect_access&&)                 = default;
  bad_expect_access& operator=(const bad_expect_access&) = default;
  bad_expect_access& operator=(bad_expect_access&&)      = default;
  ~bad_expect_access() override                          = default;

public:
  // The way this has been designed (by using a class template below) means that we'll already
  // have a profusion of these vtables in TUs, and the dynamic linker will already have a bunch
  // of work to do. So it is not worth hiding the <void> specialization in the dylib, given that
  // it adds deployment target restrictions.
  const char* what() const noexcept override { return "bad access to expected"; }
};

template <class _Err>
class bad_expect_access : public bad_expect_access<void> {
public:
   explicit bad_expect_access(_Err __e) : __unex_(std::move(__e)) {}

   _Err& error() & noexcept { return __unex_; }
   const _Err& error() const& noexcept { return __unex_; }
   _Err&& error() && noexcept { return std::move(__unex_); }
   const _Err&& error() const&& noexcept { return std::move(__unex_); }

private:
  _Err __unex_;
};

}
