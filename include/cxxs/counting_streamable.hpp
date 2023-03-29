// Copyright $year.today Karma Krafts & associates
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
 * @since 29/03/2023
 */

#pragma once

#include <optional>
#include <functional>

namespace cxxs {
    template<typename T> //
    struct CountingStreamable final {
        using value_type = T;

        private:

        T _value;
        size_t _max_count;
        size_t _count;

        public:

        constexpr CountingStreamable(T value, size_t max_count) noexcept:
                _value(std::move(value)),
                _max_count(max_count),
                _count(0) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            if (_count == _max_count) {
                return std::nullopt;
            }

            ++_count;
            return std::make_optional(_value);
        }
    };
}