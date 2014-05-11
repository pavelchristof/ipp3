#include <QtWidgets/QApplication>
#include "gui/mainwindow.hpp"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	srand(time(0));
	ipp3::gui::MainWindow window;
	window.show();
/*
	
	QFile file("/home/pawel/projects/ipp/testy jezykowe/data/test.ltf");
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);
	Parser parser;
	Document doc;
	try {
		doc = parser.parse(&stream);
	} catch (const ParserError& err) {
		std::cout << err.message().toStdString() << std::endl;
		return 1;
	}

	Model model(doc, "/home/pawel/projects/ipp/testy jezykowe/data/");

	for (auto ph : model.currentTask().choices()) {
		std::cout << "choice: " << qPrintable(ph.words().join(' ')) << std::endl;
	}

	model.insert(model.currentTask().choices().front() , model.currentTask().gaps().front());
	model.finish();

	for (auto elem : model.currentTask().text()) {
		if (elem.isLeft()) {
			std::cout << qPrintable(elem.left()) << std::endl;
		} else {
			if (elem.right().isEmpty()) {
				std::cout << "..." << std::endl;
			} else {
				std::cout << qPrintable(elem.right().phrase().words().join(' ')) << std::endl;
			}
		}
	}

	std::cout << model.currentTask().correctAnswers() << std::endl;*/

	return app.exec();
}
