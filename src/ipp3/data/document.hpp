#ifndef IPP3_DATA_DOCUMENT_HPP
#define IPP3_DATA_DOCUMENT_HPP

#include <QtCore/QString>
#include <QtCore/QVector>

#include "../either.hpp"

namespace ipp3 {
namespace data {

struct Gap {
	QString img;
	QString content;
};

struct Task {
	QVector<Either<QString, Gap>> content;
	QVector<Gap> extraGaps;
};

struct Document {
	QVector<Task> tasks;

	void clear();
};

} // namespace data
} // namespace ipp3

#endif // IPP3_DATA_DOCUMENT_HPP
