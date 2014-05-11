#ifndef IPP3_GUI_STARTSCREEN_HPP
#define IPP3_GUI_STARTSCREEN_HPP

#include <QtWidgets/QWidget>

namespace Ui
{
class StartScreen;
}

namespace ipp3 {
namespace gui {

class StartScreen : public QWidget
{
	Q_OBJECT
public:
	StartScreen();
	~StartScreen();

signals:
	void testFileChosen(const QString& fileName);

private:
	void showOpenFileDialog();

	Ui::StartScreen* ui;
};

}
}

#endif // IPP3_GUI_STARTSCREEN_HPP
