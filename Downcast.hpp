#pragma once

#include "./Ref.hpp"
#include "./Option.hpp"

namespace Nrl {
    template<typename To, typename From>
	[[nodiscard]] Option<Ref<To>> Downcast(Ref<From> from) {
		To* casted = dynamic_cast<To*>((RemoveConst_t<RemovePointer_t<From*>>*)from.ptr());
		if (!casted)
		    return None();

		return SomeWith(Ref<To>::FromPtr, casted);
	}
} // namespace Nrl
