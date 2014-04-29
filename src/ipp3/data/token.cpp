#include "token.hpp"

namespace ipp3 {
namespace data {

QString Token::toString()
{
	switch (type) {
		case TagStart:
			return "TagStart";

		case ClosingTagStart:
			return "ClosingTagStart";

		case TagEnd:
			return "TagEnd";

		case Equals:
			return "Equals";

		case Identifier:
			return "Identifier (" + data + ")";

		case Quoted:
			return "Quoted (" + data + ")";

		case Text:
			return "Text (" + data + ")";

		case EndOfFile:
			return "EndOfFile";
	}
}

}
}
