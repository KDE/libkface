/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2012-01-03
 * @brief   <a href="http://docs.opencv.org/modules/contrib/doc/facerec/facerec_tutorial.html#local-binary-patterns-histograms">Face Recognition based on Local Binary Patterns Histograms</a>
 *          Ahonen T, Hadid A. and Pietikäinen M. "Face description with local binary
 *          patterns: Application to face recognition." IEEE Transactions on Pattern
 *          Analysis and Machine Intelligence, 28(12):2037-2041.
 *
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2012-2013 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (c) 2011-2012 Philipp Wagner
 *         <a href="mailto:bytefish at gmx dot de">bytefish at gmx dot de</a>
 *
 * @section LICENSE
 *
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 *
 * ============================================================ */

#include "libopencv.h"

// C++ includes

#include <vector>

namespace KFaceIface
{

class LBPHFaceRecognizer : public cv::FaceRecognizer
{
public:

    enum PredictionStatistics
    {
        NearestNeighbor,
        NearestMean,
        MostNearestNeighbors // makes only sense if there is a threshold!
    };

public:

    // Initializes this LBPH Model. The current implementation is rather fixed
    // as it uses the Extended Local Binary Patterns per default.
    //
    // radius, neighbors are used in the local binary patterns creation.
    // grid_x, grid_y control the grid size of the spatial histograms.
    LBPHFaceRecognizer(int radius_=1, int neighbors_=8,
                       int gridx=8, int gridy=8,
                       double threshold = DBL_MAX,
                       PredictionStatistics statistics = NearestNeighbor) :
        m_grid_x(gridx),
        m_grid_y(gridy),
        m_radius(radius_),
        m_neighbors(neighbors_),
        m_threshold(threshold),
        m_statisticsMode(statistics)
    {
    }

    // Initializes and computes this LBPH Model. The current implementation is
    // rather fixed as it uses the Extended Local Binary Patterns per default.
    //
    // (radius=1), (neighbors=8) are used in the local binary patterns creation.
    // (grid_x=8), (grid_y=8) controls the grid size of the spatial histograms.
    LBPHFaceRecognizer(cv::InputArrayOfArrays src,
                       cv::InputArray labels,
                       int radius_=1, int neighbors_=8,
                       int gridx=8, int gridy=8,
                       double threshold = DBL_MAX,
                       PredictionStatistics statistics = NearestNeighbor) :
        m_grid_x(gridx),
        m_grid_y(gridy),
        m_radius(radius_),
        m_neighbors(neighbors_),
        m_threshold(threshold),
        m_statisticsMode(statistics)
    {
        train(src, labels);
    }

    ~LBPHFaceRecognizer() {}

    using cv::FaceRecognizer::save;
    using cv::FaceRecognizer::load;

    static cv::Ptr<LBPHFaceRecognizer> create(int radius=1, int neighbors=8, int grid_x=8, int grid_y=8, double threshold = DBL_MAX, PredictionStatistics statistics = NearestNeighbor);

    // Computes a LBPH model with images in src and
    // corresponding labels in labels.
    void train(cv::InputArrayOfArrays src, cv::InputArray labels);

    // Updates this LBPH model with images in src and
    // corresponding labels in labels.
    void update(cv::InputArrayOfArrays src, cv::InputArray labels);

    // Predicts the label of a query image in src.
    int predict(cv::InputArray src) const;

    // Predicts the label and confidence for a given sample.
    void predict(cv::InputArray _src, int &label, double &dist) const;

    // See FaceRecognizer::load.
    void load(const cv::FileStorage&) {}

    // See FaceRecognizer::save.
    void save(cv::FileStorage&) const {}

    // Getter functions.
    int neighbors() const { return m_neighbors; }
    int radius()    const { return m_radius;    }
    int grid_x()    const { return m_grid_x;    }
    int grid_y()    const { return m_grid_y;    }

    // NOTE: Implementation done through CV_INIT_ALGORITHM macro from OpenCV.
    cv::AlgorithmInfo* info() const;

private:

    /** Computes a LBPH model with images in src and
     *  corresponding labels in labels, possibly preserving
     *  old model data.
     */
    void train(cv::InputArrayOfArrays src, cv::InputArray labels, bool preserveData);

private:

    // NOTE: Do not use a d private internal container, this will crash OpenCV in cv::Algorithm::set()
    int                  m_grid_x;
    int                  m_grid_y;
    int                  m_radius;
    int                  m_neighbors;
    double               m_threshold;
    int                  m_statisticsMode;

    std::vector<cv::Mat> m_histograms;
    cv::Mat              m_labels;
};

} // namespace KFaceIface
