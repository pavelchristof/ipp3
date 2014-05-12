#ifndef IPP3_GUI_MAINWINDOW_HPP
#define IPP3_GUI_MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>

namespace ipp3 {
namespace gui {

class StartScreen;
class TestView;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	/**
	 * Sets up the main window with the start screen.
	 */
	MainWindow();

private:
	void testFileChosen(const QString& fileName);
	void clearContent();

	StartScreen* startScreen;
	TestView* testView;
};

}
}

#endif // IPP3_GUI_MAINWINDOW_HPP
