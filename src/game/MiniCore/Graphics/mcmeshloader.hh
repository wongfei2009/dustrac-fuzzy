// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCMESHLOADER_HPP
#define MCMESHLOADER_HPP

#include "mcmesh.hh"

#include <QString>
#include <QTextStream>

#include <functional>
#include <map>

//! A loader for .obj-formatted 3D model files.
class MCMeshLoader
{
public:

    //! Constructor.
    MCMeshLoader();

    //! Load the given .obj-file.
    bool load(QString filePath);

    const MCMesh::FaceVector & faces() const;

private:

    struct V
    {
        float x, y, z;
    };

    struct VN
    {
        float x, y, z;
    };

    struct VT
    {
        float u, v;
    };

    std::map<QString, std::function<void(QString)> > m_keyToFunctionMap;

#ifdef MC_UNIT_TEST
public:
#endif

    bool QTextStreamMXECrashWorkaround(std::string path);

    bool readStream(QTextStream & stream);

    void processLine(QString line);

    void parseV(QString line);

    void parseVN(QString line);

    void parseVT(QString line);

    void parseF(QString line);

    std::vector<V>  m_v;
    std::vector<VN> m_vn;
    std::vector<VT> m_vt;
    MCMesh::FaceVector m_faces;
};

#endif // MCMESHLOADER_HPP
