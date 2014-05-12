#include "testview.hpp"
#include "flowlayout.hpp"
#include "gap.hpp"
#include "choice.hpp"
#include "ui_testview.h"
#include "../model.hpp"

#include <QtWidgets/QLabel>
#include <QtCore/QDebug>

namespace ipp3 {
namespace gui {

TestView::TestView(Model* model) : 
	model_(model)
{
	qDebug() << "creating TestView";

	ui = new Ui::TestView();
	ui->setupUi(this);
	textLayout = new FlowLayout(ui->text);
	choiceLayout = new FlowLayout(ui->choices);
	setupButtonsGrid();
	rebuild();

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
	qDebug() << "destroying TestView";
	delete model_;
	delete textLayout;
	delete choiceLayout;
	delete ui;
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

static QString formatScore(int correct, int total, const QString& color)
{
	return QString("<span style=\"color: %3\">%1</span>/%2").arg(correct).arg(total).arg(color);
}

void TestView::rebuild()
{
	buildText();
	buildChoices();
	chosenGap = nullptr;
	chosenChoice = nullptr;
	refresh();
}

void TestView::refresh()
{
	// update "The End!" label
	bool allFinished = true;
	for (Model::Task task : model()->tasks()) {
		if (!task.isFinished()) {
			allFinished = false;
			break;
		}
	}
	ui->theEndLabel->setVisible(allFinished);

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

	// update check/next buttons
	bool finished = model()->currentTask().isFinished();
	ui->finishButton->setEnabled(!finished);
	ui->nextButton->setEnabled(model()->hasNextTask());
	ui->nextButton->setText(finished ? tr("Next") : tr("Skip"));

	// update scores
	ui->thisTestScore->setText(formatScore(model()->currentTask().correctAnswers(), 
										   model()->currentTask().gapsCount(), "green"));
	ui->totalScore->setText(formatScore(model()->correctAnswers(), model()->totalGaps(), "green"));
	ui->wrongAnswers->setText(formatScore(model()->wrongAnswers(), model()->totalGaps(), "red"));

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
		gap->refresh(chosenGap == gap);
	}
	for (Choice* choice : choices) {
		choice->refresh(chosenChoice == choice);
	}

	update();
}

void TestView::setupButtonsGrid()
{
	for (Model::Task task : model()->tasks()) {
		QPushButton* button = new QPushButton(tr("%n", nullptr, task.index() + 1));
		button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		button->setMinimumSize(10, 20);
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

Choice* TestView::addChoice(ipp3::Model::Phrase modelChoice)
{
	Choice* choice = new Choice(modelChoice);
	connect(choice, &Choice::clicked, [=] () { choiceClicked(choice); });
	choiceLayout->addWidget(choice);
	choices.insert(choice);
	return choice;
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

	if (chosenChoice) {
		// Insert a phrase to the gap.
		if (gap->modelGap().isEmpty()) {
			model()->insert(chosenChoice->modelPhrase(), gap->modelGap());
			choiceLayout->removeWidget(chosenChoice);
			choices.remove(chosenChoice);
			delete chosenChoice;
			chosenChoice = nullptr;
		}
	} else if (chosenGap) {
		// Swap a phrase between gaps (it does nothing if the gaps are equal).
		model()->swap(chosenGap->modelGap(), gap->modelGap());

		// Deselect.
		chosenGap = nullptr;
	} else {
		// Select the gap.
		chosenGap = gap;
	}

	refresh();
}

void TestView::choiceClicked(Choice* choice)
{
	if (model()->currentTask().isFinished())
		return;

	if (chosenChoice == choice) {
		chosenChoice = nullptr;
	} else {
		chosenGap = nullptr;
		chosenChoice = choice;
	}

	refresh();
}

} // namespace gui
} // namespace ipp3
