#ifndef IPP3_DATA_TOKEN_HPP
#define IPP3_DATA_TOKEN_HPP

#include <QtCore/QString>

namespace ipp3 {
namespace data {

struct Token 
{
	enum Type
	{
		/**
		 * <
		 */
		TagStart,

		/**
		 * </
		 */
		ClosingTagStart,

		/**
		 * >
		 */
		TagEnd,

		/**
		 * =
		 */
		Equals,

		/**
		 * An identifier inside a tag, matching "[a-zA-Z0-9_]+".
		 */
		Identifier,

		/**
		 * A quoted string inside a tag.
		 */
		Quoted,

		/**
		 * A piece of text.
		 */
		Text,

		/**
		 * End of file or an error.
		 */
		EndOfFile
	};

	Token() {}
	Token(Type type, QString data = QString()) : type(type), data(data) {}

	Type type;
	QString data;

	QString toString();
};

} // namespace data
} // namespace ipp3

#endif // IPP3_DATA_TOKEN_HPP
