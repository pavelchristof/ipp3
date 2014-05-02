#ifndef IPP3_LTF_PARSER_HPP
#define IPP3_LTF_PARSER_HPP

#include "document.hpp"
#include "tokenizer.hpp"

#include <memory>
#include <QtCore/QTextStream>

namespace ipp3 {
namespace ltf {

class ParserError 
{
public:
	ParserError(const QString &message);
	QString message() const;

private:
	QString message_;
};

/**
 * Parser for the language tests format.
 * 
 * The format description can be found in "docs/ltf.txt".
 */
class Parser
{
public:
	/**
	 * Parses a document from a stream. May throw a ParserError.
	 */
	Document parse(QTextStream *stream);

private:
	Token getToken();
	Token peekToken();

	QString expect(Token::Type tokenType);
	void expectIdentifier(const QString &identifier);

	Document document();
	Task task();
	void taskContent(Task& task);
	QString extra();
	Gap gap();
	QPair<QString, QString> attribute();
	QMap<QString, QString> attributes();

	Tokenizer *tokenizer;
	PeekBuffer<Token> input;
};

} // namespace ltf
} // namespace ipp3

#endif // IPP3_LTF_PARSER_HPP
