#pragma once
#include <variant>
#include "expect_base.h"
#include "expect_x_x.h"
#include "expect_o_x.h"
#include "expect_x_o.h"
#include "expect_o_o.h"

template<class T>
using result = expect<T, void>;

template<class E>
using error_code = expect<void, E>;

using success = expect<void, void>;