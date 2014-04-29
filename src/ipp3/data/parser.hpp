#ifndef IPP3_DATA_PARSER_HPP
#define IPP3_DATA_PARSER_HPP

#include "document.hpp"
#include "tokenizer.hpp"
#include "../result.hpp"

#include <memory>
#include <QtCore/QTextStream>

namespace ipp3 {
namespace data {

class Parser
{
public:
	/**
	 * Parses a document from a stream.
	 * @returns either an error message or a document.
	 */
	Result<Document> parse(QTextStream *stream);

private:
	/**
	 * Consumes a token of type @a tokenType.
	 * @returns whether it succeeded.
	 */
	bool expect(Token::Type tokenType);

	/**
	 * Consumes a token of type @a tokenType and runs a predicate on it's data.
	 * @returns whether it succeeded and the predicate returned true.
	 */
	bool expect(Token::Type, std::function<bool (const QString& data)> pred);

	Result<Document> document(Document&& doc);
	Result<Task> task();
	Result<Task> taskContent(Task&& task);
	Result<Task> extra(Task&& task);
	Result<Task> extraContent(Task&& task);
	Result<Gap> gap();
	Result<Token> getToken();
	Result<Token> peekToken();

	Tokenizer *tokenizer;
	PeekBuffer<Token> input;
};

} // namespace data
} // namespace ipp3

#endif // IPP3_DATA_PARSER_HPP
