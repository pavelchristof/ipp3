#include "model.hpp"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

namespace ipp3 {

// Task

bool Model::Task::operator==(const Model::Task& t) const
{
	return model() == t.model() && index() == t.index();
}

bool Model::Task::operator!=(const Model::Task& t) const
{
	return !(*this == t);
}

const Model* Model::Task::model() const
{
	return model_;
}

int Model::Task::index() const
{
	return index_;
}

bool Model::Task::isFinished() const
{
	return data().isFinished;
}

int Model::Task::correctAnswers() const
{
	return data().correctAnswers;
}

int Model::Task::wrongAnswers() const
{
	return gapsCount() - data().correctAnswers;
}

int Model::Task::gapsCount() const
{
	return data().gapIndices.size();
}

int Model::Task::choicesCount() const
{
	return data().choiceBox.size();
}

QVector<Model::Gap> Model::Task::gaps() const
{
	QVector<Gap> gaps;
	for (int i : data().gapIndices) {
		gaps.push_back(Gap {model(), i});
	}
	return gaps;
}

QVector<Model::Phrase> Model::Task::choices() const
{
	QVector<Phrase> phrases;
	for (int i : data().choiceBox) {
		phrases.push_back(Phrase {model(), i});
	}
	return phrases;
}

QVector<Either<QString, Model::Gap>> Model::Task::text() const
{
	QVector<Either<QString, Model::Gap>> text;
	for (auto e : data().text) {
		if (e.isLeft()) {
			text.push_back(e.left());
		} else {
			text.push_back(Gap {model(), e.right()});
		}
	}
	return text;
}

Model::Task::Task(const Model* model, int index) :
	model_(model), index_(index)
{
}

const Model::TaskData& Model::Task::data() const
{
	return model()->tasks_[index_];
}

// Phrase

bool Model::Phrase::operator==(const Model::Phrase& p) const
{
	return model() == p.model() && index_ == p.index_;
}

bool Model::Phrase::operator!=(const Model::Phrase& p) const
{
	return !(*this == p);
}

const Model* Model::Phrase::model() const
{
	return model_;
}

Model::Task Model::Phrase::task() const
{
	return Task {model(), data().taskIndex};
}

bool Model::Phrase::isInGap() const
{
	return data().gapIndex != -1;
}

bool Model::Phrase::isInChoices() const
{
	return data().gapIndex == -1;
}

Model::Gap Model::Phrase::gap() const
{
	Q_ASSERT(isInGap());
	return Gap {model(), data().gapIndex};
}

QStringList Model::Phrase::words() const
{
	return data().words;
}

Model::Phrase::Phrase(const Model* model, int index) :
	model_(model), index_(index)
{
}

const Model::PhraseData& Model::Phrase::data() const
{
	return model()->phrases_[index_];
}

// Gap

bool Model::Gap::operator==(const Model::Gap& g) const
{
	return model() == g.model() && index_ == g.index_;
}

bool Model::Gap::operator!=(const Model::Gap& g) const
{
	return !(*this == g);
}

const Model* Model::Gap::model() const
{
	return model_;
}

Model::Task Model::Gap::task() const
{
	return Task {model(), data().taskIndex};
}

bool Model::Gap::isEmpty() const
{
	return data().phrase == -1;
}

bool Model::Gap::isCorrect() const
{
	if (isEmpty())
		return false;

	return phrase().words() == answerWords();
}

bool Model::Gap::isWrong() const
{
	return !isCorrect();
}

bool Model::Gap::hasImage() const
{
	return !data().image.isNull();
}

Model::Phrase Model::Gap::phrase() const
{
	Q_ASSERT(!isEmpty());
	return Phrase {model(), data().phrase};
}

const QImage& Model::Gap::image() const
{
	Q_ASSERT(hasImage());
	return data().image;
}

QStringList Model::Gap::answerWords() const
{
	return data().answer;
}

Model::Gap::Gap(const Model* model, int index) :
	model_(model), index_(index)
{
}

const Model::GapData& Model::Gap::data() const
{
	return model()->gaps_[index_];
}

// Model

Model::Model(const ipp3::ltf::Document& doc, const QDir& imageDir) :
	currentTask_(0), correctAnswers_(0), totalAnswers_(0)
{
	for (const ltf::Task& task : doc.tasks) {
		int taskIndex = pushTask();

		for (const Either<QString, ltf::Gap>& elem : task.content) {
			if (elem.isLeft()) {
				pushText(taskIndex, elem.left());
			} else {
				pushGap(taskIndex, elem.right(), imageDir);
			}
		}

		for (const QString& phrase : task.extra) {
			pushPhrase(taskIndex, phrase);
		}
	}

	// Sort phrases lexicographically.
	QVector<QString> joinedAndLower;
	for (PhraseData& pd : phrases_) {
		joinedAndLower.append(pd.words.join(' ').toLower());
	}
	for (TaskData& td : tasks_) {
		qSort(td.choiceBox.begin(), td.choiceBox.end(), [=] (int i, int j) {
			return joinedAndLower[i] < joinedAndLower[j];
		});
	}
}

int Model::pushTask()
{
	TaskData td;
	td.isFinished = false;
	td.correctAnswers = 0;
	tasks_.push_back(td);

	return tasks_.size() - 1;
}

void Model::pushText(int taskIndex, const QString& text)
{
	for (const QString& word : toWords(text)) {
		tasks_[taskIndex].text.push_back(word);
	}
}

void Model::pushPhrase(int taskIndex, const QString& phrase)
{
	pushPhrase(taskIndex, toWords(phrase));
}

void Model::pushPhrase(int taskIndex, const QStringList& words)
{
	int phraseIndex = phrases_.size();
	tasks_[taskIndex].choiceBox.push_back(phraseIndex);
	
	PhraseData pd;
	pd.gapIndex = -1;
	pd.words = words;
	pd.taskIndex = taskIndex;
	phrases_.push_back(pd);
}

void Model::pushGap(int taskIndex, const ltf::Gap& gap, const QDir& imageDir)
{
	QStringList words = toWords(gap.content);
	pushPhrase(taskIndex, words);

	GapData gd;
	gd.phrase = -1;
	gd.answer = words;
	gd.taskIndex = taskIndex;

	if (!gap.img.isEmpty()) {
		QFileInfo pathInfo(imageDir, gap.img);
		if (!gd.image.load(pathInfo.absoluteFilePath())) {
			// TODO: showing a message box here isn't pretty
			QMessageBox::warning(nullptr, tr("Warning"),
				tr("Cannot load image \"%1\".").arg(pathInfo.absoluteFilePath()));
		}
	}

	int gapIndex = gaps_.size();
	gaps_.push_back(gd);
	tasks_[taskIndex].gapIndices.push_back(gapIndex);
	tasks_[taskIndex].text.push_back(gapIndex);
}

QVector<Model::Task> Model::tasks() const
{
	QVector<Task> tasks;
	for (int i = 0; i < tasks_.size(); ++i) {
		tasks.push_back(Task {this, i});
	}
	return tasks;
}

void Model::switchTask(Task task)
{
	Q_ASSERT(task.model() == this);
	currentTask_ = task.index();
}

Model::Task Model::currentTask() const
{
	return Task {this, currentTask_};
}

bool Model::hasNextTask()
{
	return currentTask_ + 1 < tasks_.size();
}

Model::Task Model::nextTask()
{
	Q_ASSERT(hasNextTask());
	return Task {this, currentTask_+1};
}

int Model::correctAnswers() const
{
	return correctAnswers_;
}

int Model::wrongAnswers() const
{
	return totalAnswers_ - correctAnswers_;
}

int Model::totalGaps() const
{
	return gaps_.size();
}

int Model::insert(Phrase phrase, Gap gap)
{
	Q_ASSERT(phrase.model() == this);
	Q_ASSERT(gap.model() == this);
	Q_ASSERT(phrase.isInChoices());
	Q_ASSERT(gap.isEmpty());
	Q_ASSERT(phrase.task() == gap.task());
	Q_ASSERT(!phrase.task().isFinished());

	TaskData& td = tasks_[phrase.task().index()];
	PhraseData& pd = phrases_[phrase.index_];
	GapData& gd = gaps_[gap.index_];

	pd.gapIndex = gap.index_;
	gd.phrase = phrase.index_;

	int choiceIndex = td.choiceBox.indexOf(phrase.index_);
	td.choiceBox.removeAt(choiceIndex);

	if (gap.isCorrect()) {
		td.correctAnswers += 1;
	}

	return choiceIndex;
}

void Model::remove(Gap gap, uint insertBefore)
{
	Q_ASSERT(gap.model() == this);
	Q_ASSERT(!gap.isEmpty());
	Q_ASSERT(!gap.task().isFinished());

	Phrase phrase = gap.phrase();

	TaskData& td = tasks_[phrase.task().index()];
	PhraseData& pd = phrases_[phrase.index_];
	GapData& gd = gaps_[gap.index_];

	if (gap.isCorrect()) {
		td.correctAnswers -= 1;
	}

	td.choiceBox.insert(insertBefore, phrase.index_);

	pd.gapIndex = -1;
	gd.phrase = -1;
}

void Model::finish()
{
	TaskData& td = tasks_[currentTask_];
	td.isFinished = true;
	correctAnswers_ += td.correctAnswers;
	totalAnswers_ += td.gapIndices.size();
}

void Model::reset()
{
	TaskData& td = tasks_[currentTask_];
	td.isFinished = false;
	correctAnswers_ -= td.correctAnswers;
	totalAnswers_ -= td.gapIndices.size();

	// move phrases out of gaps
	for (Gap gap : currentTask().gaps()) {
		if (!gap.isEmpty()) {
			remove(gap);
		}
	}
}

QStringList Model::toWords(const QString& str) const
{
	static QRegExp regexp("\\s+");
	return str.trimmed().split(regexp);
}

} // namespace ipp3
