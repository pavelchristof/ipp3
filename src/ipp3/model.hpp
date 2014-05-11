#ifndef IPP3_MODEL_HPP
#define IPP3_MODEL_HPP

#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QDir>
#include <QtGui/QImage>

#include "either.hpp"
#include "ltf/document.hpp"

namespace ipp3 {

class Model
{
private:
	struct TaskData;
	struct PhraseData;
	struct GapData;

public:
	class Gap;
	class Phrase;

	class Task
	{
	public:
		Task(const Task&) = default;
		Task(Task&&) = default;
		Task& operator = (const Task&) = default;
		Task& operator = (Task&&) = default;
		bool operator == (const Task& t) const;
		bool operator != (const Task& t) const;

		/**
		 * The associated model.
		 */
		const Model* model() const;

		/**
		 * Task index starting with 0.
		 */
		int index() const;

		bool isFinished() const;
		int correctAnswers() const;
		int wrongAnswers() const;
		int gapsCount() const;
		int choicesCount() const;

		/**
		 * All gaps in the task.
		 */
		QVector<Gap> gaps() const;

		/**
		 * Choices currently available.
		 * @note order is already random.
		 */
		QVector<Phrase> choices() const;

		/**
		 * Words or gaps, in the same order as they appear in the text.
		 */
		QVector<Either<QString, Gap>> text() const;

	private:
		Task() = default;
		Task(const Model* model, int index);
		const TaskData& data() const;

		const Model* model_;
		int index_;

		friend Model;
	};

	class Phrase
	{
	public:
		Phrase(const Phrase&) = default;
		Phrase(Phrase&&) = default;
		Phrase& operator = (const Phrase&) = default;
		Phrase& operator = (Phrase&&) = default;
		bool operator == (const Phrase& p) const;
		bool operator != (const Phrase& p) const;

		/**
		 * The associated model.
		 */
		const Model* model() const;

		/**
		 * The associated task.
		 */
		Task task() const;

		/**
		 * A phrase can be either in the choice box or inside a gap.
		 */
		//@{
		bool isInGap() const;
		bool isInChoices() const;
		//@}

		/**
		 * Gap this phrase is filling.
		 * @warning Call only when isInGap().
		 */
		Gap gap() const;

		QStringList words() const;

	private:
		Phrase() = default;
		Phrase(const Model* model, int index);
		const PhraseData& data() const;

		const Model* model_;
		int index_;

		friend Model;
	};

	class Gap
	{
	public:
		Gap(const Gap&) = default;
		Gap(Gap&&) = default;
		Gap& operator = (const Gap&) = default;
		Gap& operator = (Gap&&) = default;
		bool operator == (const Gap& g) const;
		bool operator != (const Gap& g) const;

		/**
		 * The associated model.
		 */
		const Model* model() const;

		/**
		 * The associated task.
		 */
		Task task() const;

		bool isEmpty() const;
		bool isCorrect() const;
		bool isWrong() const;
		bool hasImage() const;

		/**
		 * Phrase filling this gap.
		 * @warning Call only when !isEmpty().
		 */
		Phrase phrase() const;

		/**
		 * @warning Call only when hasImage().
		 */
		const QImage& image() const;

		QStringList answerWords() const;

	private:
		Gap() = default;
		Gap(const Model* model, int index);
		const GapData& data() const;

		const Model* model_;
		int index_;

		friend Model;
	};

	/**
	 * Creates a model from a LTF document.
	 *
	 * Image paths will be resolved relative to @a imageDir.
	 */
	Model(const ltf::Document& doc, const QDir& imageDir);

	QVector<Task> tasks() const;
	void switchTask(Task task);
	Task currentTask() const;

	bool hasNextTask();
	Task nextTask();

	int correctAnswers() const;
	int wrongAnswers() const;
	int totalGaps() const;

	/**
	 * Inserts a phrase (that must be an available choice) into an empty gap.
	 * @returns former index of the phrase in the choice box.
	 */
	int insert(Phrase phrase, Gap gap);

	/**
	 * Removes a phrase from a gap and puts it back into the choices.
	 * @param insertBefore the phrase will be put back before this index.
	 */
	void remove(Gap gap, uint insertBefore = 0);

	/**
	 * Finish the current task.
	 */
	void finish();

	/**
	 * Reset the current task.
	 */
	void reset();

private:
	QStringList toWords(const QString& str) const;
	int pushTask();
	void pushText(int taskIndex, const QString& text);
	void pushPhrase(int taskIndex, const QString& phrase);
	void pushPhrase(int taskIndex, const QStringList& words);
	void pushGap(int taskIndex, const ipp3::ltf::Gap& gap, const QDir& imageDir);

	struct TaskData {
		bool isFinished;
		int correctAnswers;
		QVector<int> gapIndices;
		QVector<int> choiceBox;
		QVector<Either<QString, int>> text;
	};

	struct PhraseData {
		// -1 encodes that the phrase is the choice box.
		int gapIndex;
		QStringList words;
		int taskIndex;
	};

	struct GapData {
		// -1 encodes that the gap is empty.
		int phrase;
		// A null image encodes that the gap has no image.
		QImage image;
		QStringList answer;
		int taskIndex;
	};

	QVector<TaskData> tasks_;
	QVector<PhraseData> phrases_;
	QVector<GapData> gaps_;
	int currentTask_;
	int correctAnswers_;
	int totalAnswers_;

	friend Task;
	friend Phrase;
	friend Gap;
};

} // namespace ipp3

#endif // IPP3_MODEL_HPP
