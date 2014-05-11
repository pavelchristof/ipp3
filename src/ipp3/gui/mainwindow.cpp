#include "mainwindow.hpp"
#include "startscreen.hpp"
#include "testview.hpp"
#include "../ltf/parser.hpp"
#include "../model.hpp"

#include <QtWidgets/QMessageBox>

namespace ipp3 {
namespace gui {

MainWindow::MainWindow()
{
	startScreen = new StartScreen();
	setCentralWidget(startScreen);
	connect(startScreen, &StartScreen::testFileChosen,
			this, &MainWindow::testFileChosen);
}

void MainWindow::testFileChosen(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly)) {
		QMessageBox::critical(this, tr("Error"), tr("Cannot open the file %1.").arg(fileName));
	}

	QTextStream stream(&file);
	ltf::Document doc;
	ltf::Parser parser;
	try {
		doc = parser.parse(&stream);
	} catch (const ltf::ParserError& error) {
		QMessageBox::critical(this, tr("Error"), 
			tr("An error was encountered when reading the test file:\n%1").arg(error.message()));
	}

	QFileInfo fileInfo(file);
	Model* model = new Model(doc, fileInfo.dir());
	testView = new TestView(model);
	setCentralWidget(testView);
	startScreen->deleteLater();
	showMaximized();
	update();
}

}
}
