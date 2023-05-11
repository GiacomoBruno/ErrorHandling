#include "unexpected.h"
#include "expected_base.h"
#include "expected_monadic_operations.h"
#include "expected_value_error.h"
#include "expected_value_void.h"
#include "expected_void_error.h"

#include "expected_void_void.h"

template<class T>
using optional = gb::expected<T, void>;

using boolean = gb::expected<void, void>;

template<class E>
using with_error = gb::expected<void, E>;

template<class T, class E>
using expected = gb::expected<T, E>;


static constexpr gb::unexpect_t error{ gb::unexpect_t::do_not_use{},  gb::unexpect_t::do_not_use{}};
static constexpr gb::expect_t success{ gb::expect_t::do_not_use{},  gb::expect_t::do_not_use{}};

static constexpr boolean yes{success};
static constexpr boolean no{error};



