#include "parser.hpp"

namespace ipp3 {
namespace ltf {

ParserError::ParserError(const QString& message) :
	message_(message)
{
}

QString ParserError::message() const
{
	return message_;
}

Document Parser::parse(QTextStream* stream)
{
	// All parsing happens in the scope of this function, so the tokenizer pointer will be valid.
	Tokenizer tok(stream);
	tokenizer = &tok;

	input.reset([&tok] (Token* token) -> bool {
		if (tok.status() == Tokenizer::Status::Available) {
			(*token) = tok.read();
			return true;
		} else {
			return false;
		}
	});

	return document();
}

QString Parser::expect(Token::Type tokenType)
{
	Token token = getToken();
	if (token.type != tokenType) {
		throw ParserError("Expected a " + Token(tokenType).toString() 
			+ " token, got " + token.toString() + " instead.");
	}
	return token.data;
}

void Parser::expectIdentifier(const QString& identifier)
{
	Token token = getToken();
	if (token.type != Token::Identifier) {
		throw ParserError("Expected an identifier (\"" + identifier + "\"), got " 
			+ token.toString() + " instead.");
	}
	if (token.data != identifier) {
		throw ParserError("Expected a \"" + identifier + "\", got \"" + token.data + "\" instead.");
	}
}

Document Parser::document()
{
	Document doc;

	for (;;) {
		Token token = getToken();
		switch (token.type) {
			case Token::TagStart:
				doc.tasks.append(task());
				break;

			case Token::Text:
				// Ignore the text.
				break;

			case Token::EndOfFile:
				return doc;

			default:
				throw ParserError("Expected text or a tag, but got " + token.toString());
		}
	}
}

Task Parser::task()
{
	Task task;

	// Opening tag.
	expectIdentifier("task");
	auto attribs = attributes();
	expect(Token::TagEnd);

	// Tag content.
	taskContent(task);

	// Closing tag.
	expect(Token::ClosingTagStart);
	expectIdentifier("task");
	expect(Token::TagEnd);

	return task;
}

void Parser::taskContent(Task& task)
{
	Token token = peekToken();
	switch (token.type) {
		case Token::TagStart: {
			input.skip();
			// Parse a gap or an extra.
			token = peekToken();
			if (token.type != Token::Identifier)
				throw ParserError("Expected an identifier after '<', got " + token.toString() + " instead.");
			if (token.data == "gap") {
				task.content.append(gap());
			} else if (token.data == "extra") {
				task.extra.append(extra());
			} else {
				throw ParserError("Expected an 'extra' or 'gap' tag, but got '" + token.data + "'.");
			}
			break;
		}

		case Token::Text:
			input.skip();
			task.content.append(token.data);
			break;

		default:
			return;
	}

	return taskContent(task);
}

QString Parser::extra()
{
	expectIdentifier("extra");
	auto attribs = attributes();
	expect(Token::TagEnd);

	QString content = expect(Token::Text);

	expect(Token::ClosingTagStart);
	expectIdentifier("extra");
	expect(Token::TagEnd);

	return content;
}

Gap Parser::gap()
{
	Gap gap;

	expectIdentifier("gap");
	auto attribs = attributes();
	gap.img = attribs.value("img");
	expect(Token::TagEnd);

	gap.content = expect(Token::Text);

	expect(Token::ClosingTagStart);
	expectIdentifier("gap");
	expect(Token::TagEnd);

	return gap;
}

QPair<QString, QString> Parser::attribute()
{
	QString id = expect(Token::Identifier);
	expect(Token::Equals);
	QString val = expect(Token::Quoted);
	return qMakePair(id, val);
}

QMap<QString, QString> Parser::attributes()
{
	QMap<QString, QString> attribs;

	while (peekToken().type == Token::Identifier) {
		auto att = attribute();
		attribs.insert(att.first, att.second);
	}

	return attribs;
}

Token Parser::getToken()
{
	Token token;
	if (input.get(&token)) {
		return token;
	} else {
		// Propagate tokenizer error.
		throw ParserError{tokenizer->errorMessage()};
	}
}

Token Parser::peekToken()
{
	Token token;
	if (input.peek(&token)) {
		return token;
	} else {
		// Propagate tokenizer error.
		throw ParserError{tokenizer->errorMessage()};
	}
}

} // namespace ltf
} // namespace ipp3
