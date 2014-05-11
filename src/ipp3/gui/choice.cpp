#include "choice.hpp"
#include "gap.hpp"

#include <QtWidgets/QStackedLayout>

namespace ipp3 {
namespace gui {

Choice::Choice(Model::Phrase phrase) :
	modelPhrase_(phrase)
{
	setLayout(new QStackedLayout());
	label = new QLabel(phrase.words().join(' '));
	layout()->addWidget(label);
	refresh(false);
}

Model::Phrase Choice::modelPhrase() const
{
	return modelPhrase_;
}

void Choice::refresh(bool isChosen)
{
	QString style = "border-style:%1; border-radius: %2px; border-width: %3px; padding: %4px; ";
	style = style.arg(Gap::borderStyle).arg(Gap::borderRadius).arg(Gap::borderWidth).arg(Gap::padding);
	if (isChosen) {
		style += "background-color: LightSkyBlue";
	}
	label->setStyleSheet(style);
}

void Choice::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);
}

}
}
