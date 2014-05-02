#ifndef IPP3_LTF_DOCUMENT_HPP
#define IPP3_LTF_DOCUMENT_HPP

#include <QtCore/QString>
#include <QtCore/QVector>

#include "../either.hpp"

namespace ipp3 {
namespace ltf {

struct Gap {
	QString img;
	QString content;
};

struct Task {
	QVector<Either<QString, Gap>> content;
	QVector<QString> extra;
};

struct Document {
	QVector<Task> tasks;
};

} // namespace ltf
} // namespace ipp3

#endif // IPP3_LTF_DOCUMENT_HPP
