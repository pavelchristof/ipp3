#ifndef IPP3_RESULT_HPP
#define IPP3_RESULT_HPP

#include <QtCore/QString>
#include <functional>

#include "either.hpp"

namespace ipp3{

/**
 * Either an error message or a value.
 */
template <typename Value>
class Result : public Either<QString, Value>
{
public:
	// Inherit constructors.
	using Either<QString, Value>::Either;

	/**
	 * Calls func if the result is right, otherwise propagates the error.
	 */
	template <typename Value2>
	Result<Value2> andThen(std::function<Result<Value2> (const Value&)> func) const {
		if (isRight()) {
			return func(right());
		} else {
			return {left(), TagLeft};
		}
	}
};

}

#endif // IPP3_RESULT_HPP
