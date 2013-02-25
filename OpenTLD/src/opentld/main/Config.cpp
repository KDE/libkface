/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "Config.h"

#include "Main.h"

using namespace std;

namespace tld
{


Config::Config() :
    m_selectManuallySet(false),
    m_methodSet(false),
    m_startFrameSet(false),
    m_lastFrameSet(false),
    m_trajectorySet(false),
    m_showDetectionsSet(false),
    m_showForegroundSet(false),
    m_thetaSet(false),
    m_printResultsSet(false),
    m_camNoSet(false),
    m_imagePathSet(false),
    m_modelPathSet(false),
    m_initialBBSet(false),
    m_showOutputSet(false),
    m_exportModelAfterRunSet(false)
{
}

Config::~Config()
{
}

int Config::configure(Main *main)
{

    // main
    main->tld->trackerEnabled = m_settings.m_trackerEnabled;
    main->threshold = m_settings.m_threshold;
    main->tld->alternating = m_settings.m_alternating;
    main->tld->learningEnabled = m_settings.m_learningEnabled;
    main->seed = m_settings.m_seed;


    DetectorCascade *detectorCascade = main->tld->detectorCascade;
    detectorCascade->varianceFilter->enabled = m_settings.m_varianceFilterEnabled;
    detectorCascade->ensembleClassifier->enabled = m_settings.m_ensembleClassifierEnabled;
    detectorCascade->nnClassifier->enabled = m_settings.m_nnClassifierEnabled;

    // classifier
    detectorCascade->useShift = m_settings.m_useProportionalShift;
    detectorCascade->shift = m_settings.m_proportionalShift;
    detectorCascade->minScale = m_settings.m_minScale;
    detectorCascade->maxScale = m_settings.m_maxScale;
    detectorCascade->minSize = m_settings.m_minSize;
    detectorCascade->numTrees = m_settings.m_numTrees;
    detectorCascade->numFeatures = m_settings.m_numFeatures;
    detectorCascade->nnClassifier->thetaTP = m_settings.m_thetaP;
    detectorCascade->nnClassifier->thetaFP = m_settings.m_thetaN;

    return SUCCESS;
}

}
