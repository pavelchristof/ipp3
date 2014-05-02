#ifndef IPP3_LTF_TOKENIZER_HPP
#define IPP3_LTF_TOKENIZER_HPP

#include "token.hpp"
#include "../peekbuffer.hpp"

#include <QtCore/QString>
#include <QtCore/QQueue>
#include <QtCore/QMap>

class QTextStream;

namespace ipp3 {
namespace ltf {

/**
 * Tokenizer for the language tests format.
 * 
 * The format description can be found in "docs/ltf.txt".
 * 
 * @details
 * The tokenizer is implemented as a finite state machine. It reads input char
 * by char and pushes tokens to a queue. Input is processed as needed, keeping
 * at least one token in the queue (if possible).
 */
class Tokenizer
{
public:
	Tokenizer(QTextStream* stream);

	enum class Status
	{
		/**
		 * Guarantees that there is at least one more token to read.
		 */
		Available,
		
		/**
		 * There are no more tokens and no errors were encountered.
		 */
		Completed,
		
		/**
		 * There are no more tokens and an error was encountered.
		 */
		Failed
	};

	/**
	 * Reads a token. Can be called only when the status is Available.
	 */
	Token read();

	/**
	 * The current status.
	 */
	Status status();

	/**
	 * The error message. Can be caled only when the status is Failed.
	 */
	QString errorMessage();

private:
	enum class State
	{
		Default,
		Entity,
		LT,
		InTag,
		Quoted,
		QuotedEntity
	};

	void yield(Token::Type type, QString data = QString());
	void fail(const QString& msg);
	void finish();

	void flushText();
	void flushIdentifier();

	void step();
	void stepDefault();
	void stepEntity(QString &buffer, State cont);
	void stepLT();
	void stepInTag();
	void stepQuoted();

	QTextStream* stream;
	PeekBuffer<QChar> input;
	QQueue<Token> output;

	Status status_;
	QString errorMessage_;

	State state;
	QString text;
	QString entity;
	QString identifier;
	QString quoted;

	QMap<QString, QChar> entities;
};

} // namespace ltf
} // namespace ipp3

#endif // IPP3_LTF_TOKENIZER_HPP
