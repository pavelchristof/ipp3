#ifndef IPP3_GUI_GAP_HPP
#define IPP3_GUI_GAP_HPP

#include "../model.hpp"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractButton>

namespace ipp3 {
namespace gui {

class Gap : public QAbstractButton
{
	Q_OBJECT
public:
	static constexpr const char* borderStyle = "solid";
	static const int borderRadius = 5;
	static const int borderWidth = 2;
	static const int padding = 2;

	Gap(Model::Gap modelGap);
	Model::Gap modelGap() const;

	void refresh();

private:
	virtual void paintEvent(QPaintEvent* e);

	QLabel* label;
	QPixmap pixmap;
	Model::Gap modelGap_;
};
}
}

#endif // IPP3_GUI_GAP_HPP
