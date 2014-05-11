#include "startscreen.hpp"
#include "ui_startscreen.h"

#include <QtWidgets/QFileDialog>

namespace ipp3 {
namespace gui {

StartScreen::StartScreen()
{
	ui = new Ui::StartScreen();
	ui->setupUi(this);
	connect(ui->openTests, &QPushButton::clicked, 
			this, &StartScreen::showOpenFileDialog);
}

StartScreen::~StartScreen()
{
	delete ui;
}

void StartScreen::showOpenFileDialog()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("Open test file"), {}, tr("Language tests (*.ltf)"));

	if (!fileName.isNull()) {
		testFileChosen(fileName);
	}
}

}
}
