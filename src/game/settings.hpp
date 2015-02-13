// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "inputhandler.hpp"

#include <map>
#include <QString>

class Track;

//! Singleton settings class that wraps the use of QSettings.
class Settings
{
private:
    //! Constructor.
    Settings();

public:
    static Settings & instance();

    void saveLapRecord(const Track & track, int msecs);
    int loadLapRecord(const Track & track) const;
    void resetLapRecords();

    void saveRaceRecord(const Track & track, int msecs, int lapCount);
    int loadRaceRecord(const Track & track, int lapCount) const;
    void resetRaceRecords();

    void saveBestPos(const Track & track, int pos, int lapCount);
    int loadBestPos(const Track & track, int lapCount) const;
    void resetBestPos();

    void saveTrackUnlockStatus(const Track & track, int lapCount);
    bool loadTrackUnlockStatus(const Track & track, int lapCount) const;
    void resetTrackUnlockStatuses();

    void saveResolution(int hRes, int vRes, bool nativeResolution, bool fullScreen);
    void loadResolution(int & hRes, int & vRes, bool & nativeResolution, bool & fullScreen);
    void getResolution(int & hRes, int & vRes, bool & nativeResolution, bool & fullScreen);

    void saveKeyMapping(int player, InputHandler::InputAction action, int key);
    int loadKeyMapping(int player, InputHandler::InputAction action);

    void saveValue(QString key, int value);
    int loadValue(QString key, int defaultValue = 0);

    static QString lapCountKey();
    static QString soundsKey();
    static QString vsyncKey();

public:
    void setMenusDisabled(bool menusDisabled) {
    	m_menusDisabled = menusDisabled;
    }

    bool getMenusDisabled() const {
    	return m_menusDisabled;
    }

    void setControllerType(const QString& type) {
    	m_controllerType = type;
    }

    const QString& getControllerType() const {
    	return m_controllerType;
    }

    void setGameMode(const QString& gameMode) {
    	m_gameMode = gameMode;
    }

    const QString& getGameMode() const {
    	return m_gameMode;
    }

    const QString& getControllerPath() const {
    	return m_controllerPath;
    }

    void setControllerPath(const QString& path) {
    	m_controllerPath = path;
    }

    const QString& getCustomTrackFile() const {
    	return m_customTrackFile;
    }

    void setCustomTrackFile(const QString& customTrackFile) {
    	m_customTrackFile = customTrackFile;
    }

    //! Sets the resolution as specified at command line.
    void setTermResolution(int hRes, int vRes, bool nativeResolution, bool fullScreen) {
    	m_hRes = hRes;
    	m_vRes = vRes;
    	m_nativeResolution = nativeResolution;
    	m_fullScreen = fullScreen;
    }

    void setUseTermResolution(bool useTermResolution) {
    	m_useTermResolution = useTermResolution;
    }

private:
    QString m_controllerType;
    QString m_controllerPath;
    QString m_customTrackFile;
    bool m_menusDisabled;
    QString m_gameMode;

    int m_hRes;
    int m_vRes;
    bool m_nativeResolution;
    bool m_fullScreen;
    bool m_useTermResolution = false;

private:
    QString combineActionAndPlayer(int player, InputHandler::InputAction action);
    std::map<InputHandler::InputAction, QString> m_actionToStringMap;
};

#endif // SETTINGS_HPP
