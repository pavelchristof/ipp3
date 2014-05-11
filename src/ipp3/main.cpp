#include <QtWidgets/QApplication>
#include "gui/mainwindow.hpp"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	srand(time(0));
	ipp3::gui::MainWindow window;
	window.show();
	return app.exec();
}
