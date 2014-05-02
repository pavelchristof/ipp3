#include "tokenizer.hpp"

#include <QtCore/QTextStream>

namespace ipp3 {
namespace ltf {

Tokenizer::Tokenizer(QTextStream* stream) :
	stream(stream),
	status_(Status::Available),
	state(State::Default)
{
	input.reset([stream] (QChar* c) -> bool {
		(*stream) >> (*c); 
		return stream->status() == QTextStream::Ok;
	});

	entities.insert("lt", '<');
	entities.insert("gt", '>');
	entities.insert("amp", '&');
	entities.insert("quot", '"');

	// Read the first token into the output queue or fail trying.
	while (status_ == Status::Available && output.empty()) {
		step();
	}
}

Token Tokenizer::read()
{
	// There is at least one token in the output queue.
	Q_ASSERT(status() == Status::Available);

	// Consume input until we have 2 tokens in the ouput queue or until 
	// we encounter EOF or an error.
	while (status_ == Status::Available && output.size() < 2) {
		step();
	}

	return output.dequeue();
}

Tokenizer::Status Tokenizer::status()
{
	// Notice that status_ does not always equal status():
	// We may have encountered an error or EOF internally because we are
	// buffering output, but as long as there are tokens in the output queue
	// the user should see status as Available.
	if (!output.isEmpty())
		return Status::Available;
	return status_;
}

QString Tokenizer::errorMessage()
{
	Q_ASSERT(status() == Status::Failed);
	return errorMessage_;
}

void Tokenizer::yield(Token::Type type, QString data)
{
	output.enqueue(Token(type, data));
}

void Tokenizer::fail(const QString& msg)
{
	status_ = Status::Failed;
	errorMessage_ = msg;
}

void Tokenizer::finish()
{
	yield(Token::EndOfFile);
	status_ = Status::Completed;
}

void Tokenizer::flushText()
{
	if (!text.isEmpty()) {
		yield(Token::Text, text);
		text.clear();
	}
}

void Tokenizer::flushIdentifier()
{
	if (!identifier.isEmpty()) {
		yield(Token::Identifier, identifier);
		identifier.clear();
	}
}

void Tokenizer::step()
{
	switch (state) {
		case State::Default:
			stepDefault();
			return;

		case State::Entity:
			stepEntity(text, State::Default);
			return;

		case State::LT:
			stepLT();
			return;

		case State::InTag:
			stepInTag();
			return;

		case State::Quoted:
			stepQuoted();
			return;

		case State::QuotedEntity:
			stepEntity(quoted, State::Quoted);
			return;
	}
}

void Tokenizer::stepDefault()
{
	QChar c;

	if (!input.get(&c)) {
		flushText();
		finish();
		return;
	}

	if (c == '&') {
		state = State::Entity;
	} else if (c == '<') {
		flushText();
		state = State::LT;
	} else {
		text.append(c);
	}
}

void Tokenizer::stepEntity(QString& buffer, State cont)
{
	QChar c;

	if (!input.get(&c)) {
		fail("Unfinished entity " + entity);
		return;
	}

	if (c == ';') {
		auto it = entities.find(entity);

		if (it == entities.end()) {
			fail("Invalid entity " + entity);
		} else {
			buffer.append(it.value());
			entity.clear();
			state = cont;
		}
	} else {
		entity.append(c);
	}
}

void Tokenizer::stepLT()
{
	QChar c;

	if (!input.peek(&c)) {
		yield(Token::TagStart);
		finish();
		return;
	}

	state = State::InTag;
	if (c == '/') {
		yield(Token::ClosingTagStart);
		input.skip();
	} else {
		yield(Token::TagStart);
	}
}

void Tokenizer::stepInTag()
{
	QChar c;

	if (!input.get(&c)) {
		flushIdentifier();
		finish();
		return;
	}

	if (c == '=') {
		flushIdentifier();
		yield(Token::Equals);
	} else if (c.isSpace()) {
		flushIdentifier();
	} else if (c == '"') {
		flushIdentifier();
		state = State::Quoted;
	} else if (c == '>') {
		flushIdentifier();
		yield(Token::TagEnd);
		state = State::Default;
	} else if (c.isLetterOrNumber() || c == '_') {
		identifier.append(c);
	} else {
		fail(QString("Invalid character inside a tag: ") + c);
	}
}

void Tokenizer::stepQuoted()
{
	QChar c;

	if (!input.get(&c)) {
		fail("Unfinished quoted string: \"" + quoted + "\"");
		return;
	}

	if (c == '&') {
		state = State::QuotedEntity;
	} else if (c == '"') {
		yield(Token::Quoted, quoted);
		quoted.clear();
		state = State::InTag;
	} else {
		quoted.append(c);
	}
}

} // namespace ltf
} // namespace ipp3
