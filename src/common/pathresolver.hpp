#ifndef PATHRESOLVER_HPP
#define PATHRESOLVER_HPP

#include <QString>
#include <QStringList>
#include "export.hpp"

class DUSTCOMMON_API PathResolver {
private:
	//! The list of paths to search.
	QStringList _paths;

public:
	//! Takes in a filename or a directory name. Goes through
	//! all the paths in order, searching for it and returns
	//! the first complete path that matches or an empty string
	//! if no match is found. 
	QString resolve(const QString& subpath) const;

	//! Takes in a filename or a directory name. Goes through
	//! all the paths in order, searching for it and returns
	//! the first complete path that matches or an empty string
	//! if no match is found.
	//!
	//! The directory path is also copied into the directory
	//! argument.
	QString resolve(const QString& subpath, QString& directory) const;

public:
	PathResolver(const QStringList& paths):
		_paths(paths) {}

	PathResolver(QStringList&& paths):
		_paths(std::move(paths)) {}
};

#endif // PATHRESOLVER_HPP

