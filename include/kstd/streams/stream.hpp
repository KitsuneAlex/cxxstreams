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
 * @since 27/03/2023
 */

#pragma once

#include <optional>
#include <functional>
#include <type_traits>

#include "concepts.hpp"
#include "stream_fwd.hpp"

#include "iterator_streamable.hpp"
#include "owning_iterator_streamable.hpp"
#include "singlet_streamable.hpp"
#include "counting_streamable.hpp"
#include "draining_streamable.hpp"

#include "basic_stream.hpp"
#include "chaining_stream.hpp"
#include "filtering_stream.hpp"
#include "limiting_stream.hpp"
#include "mapping_stream.hpp"
#include "flat_mapping_stream.hpp"
#include "distinct_stream.hpp"
#include "zipping_stream.hpp"
#include "flat_zipping_stream.hpp"
#include "peeking_stream.hpp"
#include "dropping_stream.hpp"
#include "taking_stream.hpp"
#include "sorting_stream.hpp"

namespace kstd::streams {
    template<typename T, typename S, typename IMPL> //
    KSTD_REQUIRES(concepts::Streamable<S>)
    class Stream {
    protected:

        [[maybe_unused]] S _streamable;

        [[nodiscard]] constexpr auto get_self() noexcept -> IMPL& {
            #ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(concepts::Streamable<IMPL>, "Implementation is not streamable");
            #endif // KSTD_CONCEPTS_AVAILABLE
            return static_cast<IMPL&>(*this);
        }

    public:

        explicit KSTD_STREAM_CONSTRUCTOR Stream(S streamable) noexcept : _streamable(std::move(streamable)) {
        }

        template<typename S2>
        KSTD_REQUIRES(concepts::Streamable<S2>)
        [[nodiscard]] constexpr auto chain(S2 other) noexcept -> ChainingStream<IMPL, S2> {
            return ChainingStream<IMPL, S2>(std::move(get_self()), std::move(other));
        }

        template<typename S2>
        KSTD_REQUIRES(concepts::Streamable<S2>)
        [[nodiscard]] constexpr auto pre_chain(S2 other) noexcept -> ChainingStream<S2, IMPL> {
            return ChainingStream<S2, IMPL>(std::move(other), std::move(get_self()));
        }

        template<typename F>
        KSTD_REQUIRES((kstd::concepts::Function<F, void(T&)>))
        [[nodiscard]] constexpr auto filter(F&& filter) noexcept -> FilteringStream<IMPL, F> {
            return FilteringStream<IMPL, F>(std::move(get_self()), std::forward<F>(filter));
        }

        template<typename M, typename R = std::invoke_result_t<M, T&>>
        KSTD_REQUIRES((kstd::concepts::Function<M, R(T & )>))
        [[nodiscard]] constexpr auto map(M&& mapper) noexcept -> MappingStream<R, IMPL, M> {
            return MappingStream<R, IMPL, M>(std::move(get_self()), std::forward<M>(mapper));
        }

        template<typename M, typename RS = std::invoke_result_t<M, T&>>
        KSTD_REQUIRES((kstd::concepts::Function<M, RS(T & )> && concepts::Streamable<RS>))
        [[nodiscard]] constexpr auto flat_map(M&& mapper) noexcept -> FlatMappingStream<IMPL, RS, M> {
            return FlatMappingStream<IMPL, RS, M>(std::move(get_self()), std::forward<M>(mapper));
        }

        template<typename LM, typename L = std::invoke_result_t<LM, T&>, typename RM, typename R = std::invoke_result_t<RM, T&>>
        KSTD_REQUIRES((kstd::concepts::Function<LM, L(T & )> && kstd::concepts::Function<RM, R(T & )>))
        [[nodiscard]] constexpr auto zip(LM&& left_mapper, RM&& right_mapper) noexcept -> ZippingStream<IMPL, L, R, LM, RM> {
            return ZippingStream<IMPL, L, R, LM, RM>(std::move(get_self()), std::forward<LM>(left_mapper), std::forward<RM>(right_mapper));
        }

        template<typename LM, typename LS = std::invoke_result_t<LM, T&>, typename RM, typename RS = std::invoke_result_t<RM, T&>>
        KSTD_REQUIRES((kstd::concepts::Function<LM, LS(T & )> && kstd::concepts::Function<RM, RS(T & )>))
        [[nodiscard]] constexpr auto flat_zip(LM&& left_mapper, RM&& right_mapper) noexcept -> FlatZippingStream<IMPL, LS, RS, LM, RM> {
            return FlatZippingStream<IMPL, LS, RS, LM, RM>(std::move(get_self()), std::forward<LM>(left_mapper), std::forward<RM>(right_mapper));
        }

        template<typename F>
        KSTD_REQUIRES((kstd::concepts::Function<F, void(T&)>))
        [[nodiscard]] constexpr auto peek(F&& function) noexcept -> PeekingStream<IMPL, F> {
            return PeekingStream<IMPL, F>(std::move(get_self()), std::forward<F>(function));
        }

        template<typename P>
        KSTD_REQUIRES((kstd::concepts::Function<P, bool(T&)>))
        [[nodiscard]] constexpr auto drop_while(P&& predicate) noexcept -> DroppingStream<IMPL, P> {
            return DroppingStream<IMPL, P>(std::move(get_self()), std::forward<P>(predicate));
        }

        template<typename P>
        KSTD_REQUIRES((kstd::concepts::Function<P, bool(T&)>))
        [[nodiscard]] constexpr auto take_while(P&& predicate) noexcept -> TakingStream<IMPL, P> {
            return TakingStream<IMPL, P>(std::move(get_self()), std::forward<P>(predicate));
        }

        template<typename C>
        KSTD_REQUIRES((kstd::concepts::Function<C, bool(const T&, const T&)>))
        [[nodiscard]] constexpr auto sorted(C&& comparator) noexcept -> SortingStream<IMPL, C> {
            return SortingStream<IMPL, C>(std::move(get_self()), std::forward<C>(comparator));
        }

        [[nodiscard]] constexpr auto limit(size_t max_count) noexcept -> LimitingStream<IMPL> {
            return LimitingStream<IMPL>(std::move(get_self()), max_count);
        }

        [[nodiscard]] constexpr auto distinct() noexcept -> DistinctStream<IMPL> {
            return DistinctStream<IMPL>(std::move(get_self()));
        }

        [[nodiscard]] constexpr auto sorted() noexcept -> decltype(auto) {
            #ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::LessThanComparable<T>, "Stream value type doesn't implement operator< or operator>");
            #endif // KSTD_CONCEPTS_AVAILABLE

            return sorted([](const auto& a, const auto& b) {
                return a < b;
            });
        }

        [[nodiscard]] constexpr auto filter_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter([](auto& value) {
                return value != nullptr;
            });
        }

        [[nodiscard]] constexpr auto skip(size_t count) noexcept -> IMPL& {
            auto& self = get_self();

            for (size_t i = 0;
                 i < count;
                 i++) {
                if (!self.next()) {
                    break;
                }
            }

            return self;
        }

        [[nodiscard]] constexpr auto find_first() noexcept -> std::optional<T> {
            return get_self().next();
        }

        [[nodiscard]] constexpr auto find_last() noexcept -> std::optional<T> {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                auto next = self.next();

                if (!next) {
                    break;
                }

                element = std::move(next);
            }

            return element;
        }

        template<kstd::concepts::Function<T(T, T)> F>
        [[nodiscard]] constexpr auto reduce(F&& function) noexcept -> std::optional<T> {
            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto acc = std::move(*element);
            auto next = self.next();

            while (next) {
                acc = std::move(function(std::move(acc), std::move(*next)));
                next = self.next();
            }

            return std::make_optional(acc);
        }

        [[nodiscard]] constexpr auto sum() noexcept -> std::optional<T> {
            #ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::Addable<T>, "Stream value type doesn't implement operator+");
            #endif // KSTD_CONCEPTS_AVAILABLE

            return reduce([](auto a, auto b) {
                return a + b;
            });
        }

        [[nodiscard]] constexpr auto min() noexcept -> std::optional<T> {
            #ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::LessThanComparable<T>, "Stream value type doesn't implement operator<");
            #endif // KSTD_CONCEPTS_AVAILABLE

            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto result = std::move(*element);
            element = self.next();

            while (element) {
                auto value = std::move(*element);

                if (value < result) {
                    result = std::move(value);
                }

                element = self.next();
            }

            return std::make_optional(result);
        }

        [[nodiscard]] constexpr auto max() noexcept -> std::optional<T> {
            #ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::LessThanComparable<T>, "Stream value type doesn't implement operator<");
            #endif // KSTD_CONCEPTS_AVAILABLE

            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto result = std::move(*element);
            element = self.next();

            while (element) {
                auto value = std::move(*element);

                if (result < value) {
                    result = std::move(value);
                }

                element = self.next();
            }

            return std::make_optional(result);
        }

        [[nodiscard]] constexpr auto count() noexcept -> size_t {
            size_t result = 0;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                result++;
                element = self.next();
            }

            return result;
        }

        template<typename F>
        KSTD_REQUIRES((kstd::concepts::Function<F, void(T&)>))
        constexpr auto for_each(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                function(*element);
                element = self.next();
            }
        }

        template<typename F>
        KSTD_REQUIRES((kstd::concepts::Function<F, void(T&, size_t)>))
        constexpr auto for_each_indexed(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element) {
                function(*element, index++);
                element = self.next();
            }
        }

        template<typename P>
        KSTD_REQUIRES((kstd::concepts::Function<P, bool(T&)>))
        [[nodiscard]] constexpr auto all_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if (!predicate(*element)) {
                    return false;
                }

                element = self.next();
            }

            return true;
        }

        template<typename P>
        KSTD_REQUIRES((kstd::concepts::Function<P, bool(T&)>))
        [[nodiscard]] constexpr auto any_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if (predicate(*element)) {
                    return true;
                }

                element = self.next();
            }

            return false;
        }

        template<typename P>
        KSTD_REQUIRES((kstd::concepts::Function<P, bool(T&)>))
        [[nodiscard]] constexpr auto none_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if (predicate(*element)) {
                    return false;
                }

                element = self.next();
            }

            return true;
        }

        [[nodiscard]] constexpr auto deref_all() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return map([](auto& value) {
                return *value;
            });
        }

        [[nodiscard]] constexpr auto deref_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter_not_null().map([](auto& value) {
                return *value;
            });
        }

        template<template<typename, typename...> typename C>
        KSTD_REQUIRES((concepts::Pushable<C < T>> && std::is_default_constructible_v<C<T>>))
        [[nodiscard]] constexpr auto collect() noexcept -> C <T> {
            C<T> result;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                result.push_back(std::move(*element));
                element = self.next();
            }

            return result;
        }

        template<template<typename, typename, typename...> typename M, typename KM, typename K = std::invoke_result_t<KM, T&>, typename VM, typename V = std::invoke_result_t<VM, T&>>
        KSTD_REQUIRES((kstd::concepts::Function<KM, K(T & )> && kstd::concepts::Function<VM, V(T & )> && std::is_default_constructible_v<M < K, V>> && concepts::Indexable<K, V, M>))
        [[nodiscard]] constexpr auto collect_map(KM&& key_mapper, VM&& value_mapper) noexcept -> M <K, V> {
            M<K, V> result;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                auto& value = *element;
                result[key_mapper(value)] = std::move(value_mapper(value));
                element = self.next();
            }

            return result;
        }

        template<size_t EXTENT, template<typename, size_t, typename...> typename SEQ>
        requires(EXTENT > 0)
        [[nodiscard]] constexpr auto collect_sequence() noexcept -> SEQ <T, EXTENT> {
            SEQ<T, EXTENT> result;
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element && index < EXTENT) {
                result[index++] = std::move(*element);
                element = self.next();
            }

            return result;
        }

        constexpr auto collect_to_memory(T* elements, size_t max_count) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element && index < max_count) {
                *(elements + index) = std::move(*element);
                element = self.next();
                index++;
            }
        }

        [[nodiscard]] constexpr auto evaluate() noexcept -> decltype(auto) {
            return owning(collect<std::vector>());
        }

        // Chain operators (append)

        template<typename OTHER>
        KSTD_REQUIRES(concepts::Streamable<OTHER>)
        [[nodiscard]] constexpr auto operator |(OTHER other) noexcept -> decltype(auto) {
            return chain(std::move(other));
        }

        template<typename OTHER>
        KSTD_REQUIRES(kstd::concepts::ConstIterable<OTHER>)
        [[nodiscard]] constexpr auto operator |(const OTHER& container) noexcept -> decltype(auto) {
            return chain(stream(container));
        }

        template<typename OTHER>
        KSTD_REQUIRES(kstd::concepts::ConstIterable<OTHER>)
        [[nodiscard]] constexpr auto operator |(OTHER&& container) noexcept -> decltype(auto) {
            return chain(owning(std::forward(container)));
        }

        // Pre-chain operators (prepend)

        template<typename OTHER>
        KSTD_REQUIRES(concepts::Streamable<OTHER>)
        [[nodiscard]] constexpr auto operator ||(OTHER other) noexcept -> decltype(auto) {
            return pre_chain(std::move(other));
        }

        template<typename OTHER>
        KSTD_REQUIRES(kstd::concepts::ConstIterable<OTHER>)
        [[nodiscard]] constexpr auto operator ||(const OTHER& container) noexcept -> decltype(auto) {
            return pre_chain(stream(container));
        }

        template<typename OTHER>
        KSTD_REQUIRES(kstd::concepts::ConstIterable<OTHER>)
        [[nodiscard]] constexpr auto operator ||(OTHER&& container) noexcept -> decltype(auto) {
            return pre_chain(owning(std::forward(container)));
        }

        // Deref operator (find first)

        [[nodiscard]] constexpr auto operator *() noexcept -> std::optional<T> {
            return find_first();
        }
    };

    template<typename C>
    KSTD_REQUIRES(kstd::concepts::ConstIterable<C>)
    [[nodiscard]] constexpr auto stream(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>> {
        using streamable = IteratorStreamable<typename C::const_iterator>;
        return BasicStream<streamable>(streamable(container.cbegin(), container.cend()));
    }

    template<typename C>
    KSTD_REQUIRES(kstd::concepts::ConstIterable<C>)
    [[nodiscard]] constexpr auto owning(C container) noexcept -> BasicStream<OwningIteratorStreamable<C>> {
        using streamable = OwningIteratorStreamable<C>;
        return BasicStream<streamable>(streamable(std::move(container)));
    }

    template<typename C>
    KSTD_REQUIRES(kstd::concepts::ConstReverseIterable<C>)
    [[nodiscard]] constexpr auto reverse(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>> {
        using streamable = IteratorStreamable<typename C::const_iterator>;
        return BasicStream<streamable>(streamable(container.crbegin(), container.crend()));
    }

    template<typename C>
    KSTD_REQUIRES((kstd::concepts::Iterable<C> && concepts::Erasable<C>))
    [[nodiscard]] constexpr auto draining(C& container) noexcept -> BasicStream<DrainingStreamable<C>> {
        using streamable = DrainingStreamable<C>;
        return BasicStream<streamable>(streamable(container));
    }

    template<typename T>
    KSTD_REQUIRES(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto singlet(T value) noexcept -> BasicStream<SingletStreamable<T>> {
        using streamable = SingletStreamable<T>;
        return BasicStream<streamable>(streamable(std::move(value)));
    }

    template<typename T>
    KSTD_REQUIRES(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto counting(T value, size_t max_count) noexcept -> BasicStream<CountingStreamable<T>> {
        using streamable = CountingStreamable<T>;
        return BasicStream<streamable>(streamable(std::move(value), max_count));
    }

}