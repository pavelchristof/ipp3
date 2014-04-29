#include "parser.hpp"

namespace ipp3 {
namespace data {

Result<Document> Parser::parse(QTextStream* stream)
{
	// All parsing happens in the scope of this function, so the tokenizer pointer will be valid.
	Tokenizer tok(stream);
	tokenizer = &tok;

	input.reset([tokenizer] (Token& token) -> bool {
		if (tokenizer->status() == Tokenizer::Status::Available) {
			token = tokenizer->read();
			return true;
		} else {
			return false;
		}
	});

	// Start parsing with an empty document.
	return document(Document{});
}

bool Parser::expect(Token::Type tokenType)
{
	Token token;
}

bool Parser::expect(Token::Type, std::function<bool (const QString& data)> pred)
{

}


Result<Document> Parser::document(Document&& doc)
{
	return peekToken().andThen([&] (const Token& token) {
		switch (token.type) {
			case Token::TagStart:
				return task().andThen([&] (const Task& task) {
					
				});

			case Token::Text:
				// ignore top level text
				input.skip();
				return document(doc);

			default:
				return QString("Expected text or a tag, but got " + token.toString());
		}
	});
}

Result<Task> Parser::task()
{
	if (!expect(Token::TagStart)) {
		return "";
	}
}

Result<Token> Parser::getToken()
{
	Token token;
	if (input.get(token)) {
		return std::move(token);
	} else {
		// Propagate tokenizer error.
		return tokenizer->errorMessage();
	}
}

Result<Token> Parser::peekToken()
{
	Token token;
	if (input.peek(token)) {
		return std::move(token);
	} else {
		// Propagate tokenizer error.
		return tokenizer->errorMessage();
	}
}

} // namespace data
} // namespace ipp3
