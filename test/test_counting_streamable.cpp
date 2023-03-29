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

#include <vector>
#include <gtest/gtest.h>
#include <cxxs/counting_streamable.hpp>

TEST(cxxstreams_CountingStreamable, TestIterate) {
    std::vector<float> values({3.0F, 3.0F, 3.0F, 3.0F, 3.0F, 3.0F, 3.0F, 3.0F, 3.0F, 3.0F});
    auto streamable = cxxs::CountingStreamable(3.0F, values.size());

    for (const auto value : values) {
        auto element = streamable.next();
        ASSERT_TRUE(element);
        ASSERT_EQ(*element, value);
    }

    auto element = streamable.next();
    ASSERT_FALSE(element);
}