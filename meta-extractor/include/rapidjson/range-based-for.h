#include "document.h"

// see https://github.com/miloyip/rapidjson/issues/162
namespace rapidjson {
    template <typename Encoding, typename Allocator>
    typename GenericValue<Encoding,Allocator>::ValueIterator begin(GenericValue<Encoding,Allocator>& v) { return v.Begin(); }
    template <typename Encoding, typename Allocator>
    typename GenericValue<Encoding,Allocator>::ConstValueIterator begin(const GenericValue<Encoding,Allocator>& v) { return v.Begin(); }

    template <typename Encoding, typename Allocator>
    typename GenericValue<Encoding,Allocator>::ValueIterator end(GenericValue<Encoding,Allocator>& v) { return v.End(); }
    template <typename Encoding, typename Allocator>
    typename GenericValue<Encoding,Allocator>::ConstValueIterator end(const GenericValue<Encoding,Allocator>& v) { return v.End(); }
}
