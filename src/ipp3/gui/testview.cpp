#include "testview.hpp"
#include "flowlayout.hpp"
#include "gap.hpp"
#include "choice.hpp"
#include "ui_testview.h"
#include "../model.hpp"

#include <QtWidgets/QLabel>

namespace ipp3 {
namespace gui {

TestView::TestView(Model* model) : 
	model_(model)
{
	ui = new Ui::TestView();
	ui->setupUi(this);
	textLayout = new FlowLayout(ui->text);
	choiceLayout = new FlowLayout(ui->choices);
	setupButtonsGrid();
	switchTask(model->currentTask());

	// buttons
	connect(ui->finishButton, &QPushButton::clicked, [=] () { 
		model->finish(); 
		refresh();
	});
	connect(ui->resetButton, &QPushButton::clicked, [=] () {
		model->reset();
		rebuild();
	});
	connect(ui->nextButton, &QPushButton::clicked, [=] () {
		switchTask(model->nextTask());
	});
}

TestView::~TestView()
{
	delete model_;
}

Model* TestView::model()
{
	return model_;
}

void TestView::switchTask(Model::Task task)
{
	model()->switchTask(task);
	rebuild();
}

static QString formatScore(int correct, int total)
{
	return QString("<span style=\"color: green\">%1</span>/%2").arg(correct).arg(total);
}

void TestView::rebuild()
{
	buildText();
	buildChoices();
	chosen = nullptr;
	refresh();
}

void TestView::refresh()
{
	// update background color
	QString style = "background-color: White;";
	if (model()->currentTask().isFinished()) {
		ui->choices->setStyleSheet("");
		ui->text->setStyleSheet("");
	} else {
		ui->choices->setStyleSheet(style);
		ui->text->setStyleSheet(style);
	}

	// update buttons grid
	for (auto pair : buttons) {
		pair.first->setEnabled(pair.second != model()->currentTask());

		if (pair.second.isFinished()) {
			pair.first->setStyleSheet("background-color: DarkGrey");
		} else {
			pair.first->setStyleSheet("");
		}
	}

	// update check/reset/next buttons
	bool finished = model()->currentTask().isFinished();
	ui->finishButton->setEnabled(!finished);
	ui->resetButton->setEnabled(finished);
	ui->nextButton->setEnabled(model()->hasNextTask());

	// update scores
	ui->thisTestScore->setText(formatScore(model()->currentTask().correctAnswers(), 
										   model()->currentTask().gapsCount()));
	ui->totalScore->setText(formatScore(model()->correctAnswers(), model()->totalGaps()));

	// update score visibility
	if (model()->currentTask().isFinished()) {
		ui->thisTestScoreLabel->show();
		ui->thisTestScore->show();
	} else {
		ui->thisTestScoreLabel->hide();
		ui->thisTestScore->hide();
	}

	// refresh gaps and choices
	for (Gap* gap : gaps) {
		gap->refresh();
	}
	for (Choice* choice : choices) {
		choice->refresh(choice == chosen);
	}

	update();
}

void TestView::setupButtonsGrid()
{
	for (Model::Task task : model()->tasks()) {
		QPushButton* button = new QPushButton(tr("%n", nullptr, task.index() + 1));
		button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		connect(button, &QPushButton::clicked, [=] () { switchTask(task); });
		ui->buttonsGrid->addWidget(button, task.index() / 4, task.index() % 4);
		buttons.push_back(qMakePair(button, task));
	}
}

void TestView::clearChoices()
{
	choiceLayout->clear();
	for (Choice* choice : choices) {
		delete choice;
	}
	choices.clear();
}

void TestView::clearText()
{
	textLayout->clear();
	for (QLabel* label : words) {
		delete label;
	}
	words.clear();
	for (Gap* gap : gaps) {
		delete gap;
	}
	gaps.clear();
}

void TestView::buildChoices()
{
	clearChoices();

	for (Model::Phrase modelChoice : model()->currentTask().choices()) {
		addChoice(modelChoice);
	}
}

void TestView::buildText()
{
	clearText();

	for (auto elem : model()->currentTask().text()) {
		if (elem.isLeft()) {
			addWord(elem.left());
		} else {
			addGap(elem.right());
		}
	}
}

void TestView::addChoice(Model::Phrase modelChoice)
{
	Choice* choice = new Choice(modelChoice);
	connect(choice, &Choice::clicked, [=] () { choiceClicked(choice); });
	choiceLayout->addWidget(choice);
	choices.insert(choice);
}

void TestView::addWord(const QString& word)
{
	QLabel* label = new QLabel(word);
	QString style = "padding-top: %1px; padding-bottom: %1px";
	style = style.arg(Gap::borderWidth + Gap::padding);
	label->setStyleSheet(style);
	textLayout->addWidget(label);
	words.insert(label);
}

void TestView::addGap(Model::Gap modelGap)
{
	Gap* gap = new Gap(modelGap);
	connect(gap, &Gap::clicked, [=] () { gapClicked(gap); });
	textLayout->addWidget(gap);
	gaps.insert(gap);
}

void TestView::gapClicked(Gap* gap)
{
	if (model()->currentTask().isFinished())
		return;

	// Remove a phrase from the gap.
	if (!gap->modelGap().isEmpty()) {
		Model::Phrase phrase = gap->modelGap().phrase();
		model()->remove(gap->modelGap(), gap->modelGap().task().choicesCount());
		addChoice(phrase);
	}

	// Insert a phrase to the gap.
	if (chosen) {
		model()->insert(chosen->modelPhrase(), gap->modelGap());
		choiceLayout->removeWidget(chosen);
		choices.remove(chosen);
		delete chosen;
	}

	chosen = nullptr;
	refresh();
}

void TestView::choiceClicked(Choice* choice)
{
	if (model()->currentTask().isFinished())
		return;

	if (chosen == choice) {
		chosen = nullptr;
	} else {
		chosen = choice;
	}
	refresh();
}

} // namespace gui
} // namespace ipp3
