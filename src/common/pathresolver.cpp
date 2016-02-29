#include "pathresolver.hpp"
#include <QDir>

QString PathResolver::resolve(const QString& subpath) const {
	for(auto& path: _paths) {
		QDir dir(path);

		if(dir.exists(subpath)) {
			return dir.filePath(subpath);
		}
	}

	return "";
}

QString PathResolver::resolve(const QString& subpath, QString& directory) const {
	for(auto& path: _paths) {
		QDir dir(path);

		if(dir.exists(subpath)) {
			directory = path;
			return dir.filePath(subpath);
		}
	}

	directory = "";
	return "";
}

