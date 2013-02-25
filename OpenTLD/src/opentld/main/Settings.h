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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>

namespace tld
{

/**
 * In this class all settings are stored.
 */
class Settings
{
public:
    /**
     * Standard-Constructor
     */
    Settings();
    ~Settings();
    bool m_trackerEnabled;
    bool m_varianceFilterEnabled;
    bool m_ensembleClassifierEnabled;
    bool m_nnClassifierEnabled;
    bool m_useProportionalShift; //!< sets scanwindows off by a percentage value of the window dimensions (specified in proportionalShift) rather than 1px.
    bool m_loadModel; //!< if true, model specified by "modelPath" is loaded at startup
    bool m_selectManually; //!< if true, user can select initial bounding box (which then overrides the setting "initialBoundingBox")
    bool m_learningEnabled; //!< enables learning while processing
    bool m_alternating; //!< if set to true, detector is disabled while tracker is running.
    bool m_exportModelAfterRun; //!< if set to true, model is exported after run.
    int m_trajectory; //!< specifies the number of the last frames which are considered by the trajectory; 0 disables the trajectory
    int m_method; //!< method of capturing: IMACQ_CAM, IMACQ_IMGS or IMACQ_VID
    int m_startFrame; //!< first frame of capturing
    int m_lastFrame; //!< last frame of caputing; 0 means take all frames
    int m_minScale; //!< number of scales smaller than initial object size
    int m_maxScale; //!< number of scales larger than initial object size
    int m_numFeatures; //!< number of features
    int m_numTrees; //!< number of trees
    float m_thetaP;
    float m_thetaN;
    int m_seed;
    int m_minSize; //!< minimum size of scanWindows
    float m_fps; //!< Frames per second
    float m_threshold; //!< threshold for determining positive results
    float m_proportionalShift; //!< proportional shift
    std::vector<int> m_initialBoundingBox; //!< Initial Bounding Box can be specified here
};

}

#endif /* SETTINGS_H */
