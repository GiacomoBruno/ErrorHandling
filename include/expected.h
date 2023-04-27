#pragma once

#include "expected_base.h"
#include "expected_void_void.h"
#include "expected_void_error.h"
#include "expected_value_void.h"

namespace gb
{
    template<class T>
    using optional = gb::expected<T, void>;


}