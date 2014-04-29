#include <QtWidgets/QApplication>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <iostream>
#include "data/tokenizer.hpp"
#include "either.hpp"

using namespace ipp3::data;

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QFile file("data/test.ltf");
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);
	Tokenizer tokenizer(&stream);
	Token token;
	while ((token = tokenizer.read()).type != Token::Type::EndOfFile) {
		std::cout << token.toString().toStdString() << std::endl;
	}
	switch (tokenizer.status()) {
		case Tokenizer::Status::Available:
			std::cout << "Not all input consumed!!!" << std::endl;
			break;

		case Tokenizer::Status::Completed:
			std::cout << "OK." << std::endl;
			break;

		case Tokenizer::Status::Failed:
			std::cout << "Error: " << tokenizer.errorMessage().toStdString() << std::endl;
			break;
	}
	return app.exec();
}
