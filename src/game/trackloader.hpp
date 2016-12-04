// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef TRACKLOADER_HPP
#define TRACKLOADER_HPP

#include <QString>
#include <vector>

#include "difficultyprofile.hpp"
#include "tracktile.hpp"
#include "trackobjectfactory.hpp"

#include "../common/targetnodebase.hpp"

class TargetNodeBase;
class Track;
class TrackData;
class TrackTileBase;
class QDomElement;

//! A singleton class that handles track loading.
//! TODO: This can be shared with the editor or inherit from
//! a shared loader.
class TrackLoader
{
public:

    /*! Constructor.
     *  \param objectFactory  Object factory that creates objects other than tiles. */
    TrackLoader(MCObjectFactory  & objectFactory);

    //! Destructor.
    ~TrackLoader();

    //! Add path to search level files for.
    void addTrackSearchPath(QString path);

    /**
     * Returns a const reference to the vector of track search paths.
     */
    const std::vector<QString>& getTrackSearchPaths() const {
    	return m_paths;
    }

    /*! Load all tracks found in the added paths.
     *  Lock/unlock tracks according to the given lap count.
     *  \return Number of track loaded. */
    int loadTracks(int lapCount, DifficultyProfile::Difficulty difficulty);

    //! Update locked tracks when lap count changes.
    void updateLockedTracks(int lapCount, DifficultyProfile::Difficulty difficulty);

    //! Get track count.
    unsigned int tracks() const;

    //! Get a track of given index.
    Track * track(unsigned int index) const;

    static TrackLoader & instance();

    //! Load the given track.
    //! @isFullPath Specifies whether path should be understood as the full
    //! path to the file (default) or just its name.
    //! \return Valid data pointer or nullptr if fails.
    TrackData * loadTrack(QString path, bool isFullPath = true) const;

private:

    void sortTracks();

    //! Read a tile element.
    void readTile(QDomElement & element, TrackData & newData) const;

    //! Read an object element.
    void readObject(QDomElement & element, TrackData & newData) const;

    //! Read a target node element and push to the given vector.
    void readTargetNode(QDomElement & element, TrackData & newData, std::vector<TargetNodePtr> & route) const;

    //! Convert tile type string to a type enum.
    TrackTile::TileType tileTypeEnumFromString(std::string str) const;

    TrackObjectFactory m_trackObjectFactory;

    std::vector<QString> m_paths;

    std::vector<Track *> m_tracks;

    static TrackLoader * m_instance;
};

#endif // TRACKLOADER_HPP
