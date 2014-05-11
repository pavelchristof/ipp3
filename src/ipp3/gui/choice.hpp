#ifndef IPP3_GUI_CHOICE_HPP
#define IPP3_GUI_CHOICE_HPP

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractButton>

#include "../model.hpp"

namespace ipp3 {
namespace gui {

class Choice : public QAbstractButton
{
	Q_OBJECT
public:
	Choice(Model::Phrase phrase);
	Model::Phrase modelPhrase() const;

	void refresh(bool isChosen);

private:
	virtual void paintEvent(QPaintEvent* e);

	QLabel* label;
	Model::Phrase modelPhrase_;
};

}
}

#endif // IPP3_GUI_CHOICE_HPP
