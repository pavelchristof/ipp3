#include "gap.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QStackedLayout>

namespace ipp3 {
namespace gui {

Gap::Gap(Model::Gap modelGap) :
	modelGap_(modelGap)
{
	setLayout(new QStackedLayout());
	label = new QLabel();
	layout()->addWidget(label);
	if (modelGap.hasImage()) {
		pixmap = QPixmap::fromImage(modelGap.image()).scaled(200, 50, Qt::KeepAspectRatio);
	}
	refresh();
}

Model::Gap Gap::modelGap() const
{
	return modelGap_;
}

void Gap::refresh()
{
	QString style = "border-style:%1; border-radius: %2px; border-width: %3px; padding: %4px; ";
	style = style.arg(Gap::borderStyle).arg(Gap::borderRadius).arg(Gap::borderWidth).arg(Gap::padding);

	if (modelGap().isEmpty()) {
		label->setStyleSheet("");
		if (modelGap().hasImage()) {
			label->setPixmap(pixmap);
		} else {
			label->setText(".....");
		}
	} else {
		label->setText(modelGap().phrase().words().join(' '));
	}

	if (modelGap().task().isFinished()) {
		if (modelGap().isCorrect()) {
			style += "background-color: LightGreen;";
		} else {
			style += "background-color: OrangeRed;";
		}
	}

	label->setStyleSheet(style);
}

void Gap::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);
}

}
}
