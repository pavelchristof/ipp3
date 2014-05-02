#include <QtWidgets/QApplication>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <iostream>
#include "ltf/parser.hpp"

using namespace ipp3;
using namespace ipp3::ltf;

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QFile file("/home/pawel/projects/ipp/testy jezykowe/data/test.ltf");
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);
	Parser parser;
	Document doc;
	try {
		doc = parser.parse(&stream);
	} catch (const ParserError& err) {
		std::cout << err.message().toStdString() << std::endl;
	}
	return 0;//app.exec();
}
