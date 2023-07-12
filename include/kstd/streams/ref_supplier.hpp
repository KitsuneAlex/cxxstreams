// Copyright 2023 Karma Krafts & associates
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @author Alexander Hinze
 * @since 12/07/2023
 */

#pragma once

#include "kstd/defaults.hpp"
#include "kstd/option.hpp"
#include <type_traits>

namespace kstd::streams {
    template<typename I>
    struct RefSupplier final {
        // clang-format off
        using iterator      = I;
        using value_type    = typename iterator::value_type;
        using out_type      = std::conditional_t<std::is_reference_v<value_type>, value_type, value_type&>;
        using self          = RefSupplier<iterator>;
        // clang-format on

        private:
        iterator _current;
        iterator _end;

        public:
        KSTD_DEFAULT_MOVE_COPY(RefSupplier, self, constexpr)

        RefSupplier(iterator begin, iterator end) noexcept :
                _current(begin),
                _end(end) {
        }

        ~RefSupplier() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<out_type> {
            if(_current == _end) {
                return {};
            }
            return *(_current++);
        }
    };
}// namespace kstd::streams