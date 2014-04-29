#ifndef IPP3_DATA_PEEKBUFFER_HPP
#define IPP3_DATA_PEEKBUFFER_HPP

#include <functional>

namespace ipp3 {
namespace data {

template <typename Element>
class PeekBuffer
{
public:
	typedef std::function<bool (Element&)> Source;

	PeekBuffer() : isStored(false) {}
	PeekBuffer(const Source& src) : isStored(false), receive(src) {}

	bool get(Element& element) {
		if (isStored) {
			element = buffer;
			isStored = false;
			return true;
		}

		return receive(element);
	}

	bool peek(Element& element) {
		if (isStored || receive(buffer)) {
			isStored = true;
			element = buffer;
			return true;
		} else {
			return false;
		}
	}

	void skip() {
		if (isStored) {
			isStored = false;
		} else {
			receive(buffer);
		}
	}

	void reset(const Source& src) {
		isStored = false;
		receive = src;
	}

private:
	bool isStored;
	Element buffer;
	Source receive;
};

} // namespace data
} // namespace ipp3

#endif // IPP3_DATA_PEEKBUFFER_HPP
