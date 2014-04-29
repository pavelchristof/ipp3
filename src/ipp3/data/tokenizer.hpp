#ifndef IPP3_DATA_TOKENIZER_HPP
#define IPP3_DATA_TOKENIZER_HPP

#include "token.hpp"
#include "peekbuffer.hpp"

#include <QtCore/QString>
#include <QtCore/QQueue>
#include <QtCore/QMap>

class QTextStream;

namespace ipp3 {
namespace data {

class Tokenizer
{
public:
	Tokenizer(QTextStream* stream);

	enum class Status
	{
		/**
		 * There are tokens to read.
		 */
		Available,
		
		/**
		 * The stream has completed with no errors.
		 */
		Completed,
		
		/**
		 * An error was encountered.
		 */
		Failed
	};

	Token read();
	Status status();
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

} // namespace data
} // namespace ipp3

#endif // IPP3_DATA_TOKENIZER_HPP
