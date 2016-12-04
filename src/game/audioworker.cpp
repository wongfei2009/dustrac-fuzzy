// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "audioworker.hpp"
#include "audiosource.hpp"
#include "openalwavdata.hpp"
#include "openaloggdata.hpp"
#include "settings.hpp"

#include <QDir>
#include <QFile>
#include <QString>

#include <sstream>

#include <AL/al.h>

static const int MAX_DIST       = 250;
static const int REFERENCE_DIST = 50;

AudioWorker::AudioWorker(int numCars, bool enabled)
    : m_openALDevice(new OpenALDevice)
    , m_inited(false)
    , m_defaultVolume(1.0)
    , m_numCars(numCars)
    , m_enabled(enabled)
{
}

void AudioWorker::init()
{
    m_openALDevice->initialize(); // Throws on failure

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    alSpeedOfSound(1000.0);
}

void AudioWorker::checkFile(QString path)
{
    if (!QFile(path).exists())
    {
        throw std::runtime_error("File not found: '" + path.toStdString() + "'");
    }
}

bool AudioWorker::enabled() const
{
    return m_enabled;
}

void AudioWorker::connectAudioSource(AudioSource & source)
{
    connect(&source, SIGNAL(playRequested(QString, bool)),
        this, SLOT(playSound(QString, bool)));
    connect(&source, SIGNAL(stopRequested(QString)),
        this, SLOT(stopSound(QString)));
    connect(&source, SIGNAL(pitchChanged(QString, float)),
        this, SLOT(setPitch(QString, float)));
    connect(&source, SIGNAL(volumeChanged(QString, float)),
        this, SLOT(setVolume(QString, float)));
    connect(&source, SIGNAL(locationChanged(QString, float, float)),
        this, SLOT(setLocation(QString, float, float)));
}

void AudioWorker::disconnectAudioSource(AudioSource & source)
{
    disconnect(&source, SIGNAL(playRequested(QString, bool)),
        this, SLOT(playSound(QString, bool)));
    disconnect(&source, SIGNAL(stopRequested(QString)),
        this, SLOT(stopSound(QString)));
    disconnect(&source, SIGNAL(pitchChanged(QString, float)),
        this, SLOT(setPitch(QString, float)));
    disconnect(&source, SIGNAL(volumeChanged(QString, float)),
        this, SLOT(setVolume(QString, float)));
    disconnect(&source, SIGNAL(locationChanged(QString, float, float)),
        this, SLOT(setLocation(QString, float, float)));
}

void AudioWorker::loadSounds()
{
    loadCommonSound("bell", "bell.ogg");
    loadCommonSound("cheering", "cheering.ogg");
    loadCommonSound("menuBoom", "menuBoom.ogg");
    loadCommonSound("menuClick", "menuClick.ogg");
    loadCommonSound("pit", "pit.ogg");

    loadMultiInstanceCarSound("carEngine", "carEngine.ogg");
    loadMultiInstanceCarSound("carHit", "carHit.ogg");
    loadMultiInstanceCarSound("skid", "skid.ogg");

    loadSingleInstanceCarSound("carHit2", "carHit2.ogg");
    loadSingleInstanceCarSound("carHit3", "carHit3.ogg");
}

void AudioWorker::loadSingleInstanceCarSound(QString handle, QString path)
{
    const QString soundPath =
        QString(DATA_PATH) + QDir::separator() + "sounds" + QDir::separator() + path;
    checkFile(soundPath);

    STFH::SourcePtr source(new OpenALSource(
        STFH::DataPtr(new OpenALOggData(soundPath.toStdString()))));
    source->setMaxDist(MAX_DIST);
    source->setReferenceDist(REFERENCE_DIST);
    m_soundMap[handle] = source;
}

void AudioWorker::loadCommonSound(QString handle, QString path)
{
    const QString soundPath =
        QString(DATA_PATH) + QDir::separator() + "sounds" + QDir::separator() + path;
    checkFile(soundPath);

    m_soundMap[handle] =
        STFH::SourcePtr(new OpenALSource(
            STFH::DataPtr(new OpenALOggData(soundPath.toStdString()))));
}

void AudioWorker::loadMultiInstanceCarSound(QString baseName, QString path)
{
    const QString soundPath =
        QString(DATA_PATH) + QDir::separator() + "sounds" + QDir::separator() + path;
    checkFile(soundPath);

    STFH::DataPtr sharedData(new OpenALOggData(soundPath.toStdString()));

    for (int i = 0; i < m_numCars; i++)
    {
        std::stringstream ss;
        ss << baseName.toStdString() << i;
        STFH::SourcePtr source(new OpenALSource(sharedData));
        m_soundMap[ss.str().c_str()] = source;
        source->setMaxDist(MAX_DIST);
        source->setReferenceDist(REFERENCE_DIST);
    }
}

void AudioWorker::playSound(const QString & handle, bool loop)
{
    if (m_soundMap.count(handle) && m_enabled)
        m_soundMap[handle]->play(loop);
}

void AudioWorker::stopSound(const QString & handle)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->stop();
}

void AudioWorker::setPitch(const QString & handle, float pitch)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->setPitch(pitch);
}

void AudioWorker::setVolume(const QString & handle, float volume)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->setVolume(volume);
}

void AudioWorker::setDefaultVolume(float volume)
{
    m_defaultVolume = volume;
}

void AudioWorker::setLocation(const QString & handle, float x, float y)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->setLocation(STFH::Location(x, y));
}

void AudioWorker::setListenerLocation(float x, float y)
{
    alListener3f(AL_POSITION, x, y, 1);
}

void AudioWorker::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

AudioWorker::~AudioWorker()
{
}
