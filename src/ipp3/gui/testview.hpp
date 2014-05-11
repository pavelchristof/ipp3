#ifndef IPP3_GUI_TESTVIEW_HPP
#define IPP3_GUI_TESTVIEW_HPP

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QSet>

#include "../model.hpp"

namespace Ui {
class TestView;
}

namespace ipp3 {
namespace gui {
class FlowLayout;
class Gap;
class Choice;

class TestView : public QMainWindow
{
	Q_OBJECT
public:
	TestView(Model* model);
	~TestView();

	Model* model();

	void switchTask(Model::Task task);

private:
	void rebuild();
	void refresh();

	void setupButtonsGrid();

	void clearChoices();
	void clearText();

	void buildChoices();
	void buildText();

	void addChoice(Model::Phrase modelChoice);
	void addWord(const QString& word);
	void addGap(Model::Gap modelGap);

	void gapClicked(Gap* gap);
	void choiceClicked(Choice* choice);

	Model* model_;
	Ui::TestView* ui;

	Choice* chosen;
	FlowLayout* textLayout;
	FlowLayout* choiceLayout;
	QSet<QLabel*> words;
	QSet<Gap*> gaps;
	QSet<Choice*> choices;

	QVector<QPair<QPushButton*, Model::Task>> buttons;
};

} // namespace gui
} // namespace ipp3

#endif // IPP3_GUI_TESTVIEW_HPP

