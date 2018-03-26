#ifndef CPP_STREAM_DETAIL_STREAM_TRAITS_IMPL_H
#define CPP_STREAM_DETAIL_STREAM_TRAITS_IMPL_H

#include "utility.h"        // VoidT
#include "traits_impl.h"    // RemoveCRefT, InvokeResultT
#include <type_traits>      // ::std::enable_if_t, ::std::remove_reference_t, ::std::decay_t
#include <optional>         // ::std::optional

namespace stream
{
    template<typename T1, typename T2>
    struct Stream;

    namespace detail
    {
        template<typename Stream>
        struct StreamTraitsImpl
        {
            static constexpr bool IsStream = false;
        };

        template<typename T1, typename T2>
        struct StreamTraitsImpl<stream::Stream<T1, T2>>
        {
            static constexpr bool IsStream = true;
            using StreamType = stream::Stream<T1, T2>;
            using ValueType = typename StreamType::Type;
            using Source = T2;
            static constexpr bool IsFinite = StreamType::IsFinite;
        };

        template<typename Stream>
        struct StreamTraits : StreamTraitsImpl<RemoveCRefT<Stream>>
        {};


        template<typename Stream>
        constexpr static bool IsStreamV = StreamTraits<Stream>::IsStream;

        template<typename Stream>
        using StreamTypeT = typename StreamTraits<Stream>::StreamType;

        template<typename Stream>
        using StreamValueT = typename StreamTraits<Stream>::ValueType;

        template<typename Stream>
        using StreamSourceT = typename StreamTraits<Stream>::Source;

        template<typename Stream>
        constexpr static bool StreamFinitenessV = StreamTraits<Stream>::IsFinite;


        template<typename Filter, typename S, typename = void>
        struct IsStreamFilterForImpl : ::std::false_type
        {};

        template<typename Filter, typename T1, typename T2>
        struct IsStreamFilterForImpl<Filter, stream::Stream<T1,T2>,
                                     VoidT<::std::enable_if_t<::std::is_same<::std::optional<typename InvokeResultT<::std::decay_t<Filter>,
                                                                                                                    const StreamValueT<stream::Stream<T1, T2>>&,
                                                                                                                    const stream::Stream<T1, T2>&>::value_type>,
                                                                             InvokeResultT<::std::decay_t<Filter>,
                                                                                           const StreamValueT<stream::Stream<T1, T2>>&,
                                                                                           const stream::Stream<T1, T2>&>>::value>>
                                    > : ::std::true_type
        {};


        template<typename Filter, typename S>
        struct IsStreamFilterFor : public IsStreamFilterForImpl<Filter, RemoveCRefT<S>>
        {};
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_TRAITS_IMPL_H