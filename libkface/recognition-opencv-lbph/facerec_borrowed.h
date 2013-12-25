/*
 * Copyright (c) 2011,2012. Philipp Wagner <bytefish[at]gmx[dot]de>.
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
 */
#include "libopencv.h"
#include <vector>

namespace KFaceIface
{


// Face Recognition based on Local Binary Patterns.
//
//  Ahonen T, Hadid A. and Pietikäinen M. "Face description with local binary
//  patterns: Application to face recognition." IEEE Transactions on Pattern
//  Analysis and Machine Intelligence, 28(12):2037-2041.
//
class LBPHFaceRecognizer : public cv::FaceRecognizer
{
public:
    
    enum PredictionStatistics
    {
        NearestNeighbor,
        NearestMean,
        MostNearestNeighbors // makes only sense if there is a threshold!
    };
private:
    int _grid_x;
    int _grid_y;
    int _radius;
    int _neighbors;
    double _threshold;
    int _statisticsMode;

    std::vector<cv::Mat> _histograms;
    cv::Mat _labels;

    // Computes a LBPH model with images in src and
    // corresponding labels in labels, possibly preserving
    // old model data.
    void train(cv::InputArrayOfArrays src, cv::InputArray labels, bool preserveData);

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
        _grid_x(gridx),
        _grid_y(gridy),
        _radius(radius_),
        _neighbors(neighbors_),
        _threshold(threshold),
        _statisticsMode(statistics) {}

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
                _grid_x(gridx),
                _grid_y(gridy),
                _radius(radius_),
                _neighbors(neighbors_),
                _threshold(threshold),
                _statisticsMode(statistics) {
        train(src, labels);
    }

    using cv::FaceRecognizer::save;
    using cv::FaceRecognizer::load;

    static cv::Ptr<LBPHFaceRecognizer> create(int radius=1, int neighbors=8, int grid_x=8, int grid_y=8, double threshold = DBL_MAX, PredictionStatistics statistics = NearestNeighbor);
    

    ~LBPHFaceRecognizer() { }

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
    int neighbors() const { return _neighbors; }
    int radius() const { return _radius; }
    int grid_x() const { return _grid_x; }
    int grid_y() const { return _grid_y; }

    cv::AlgorithmInfo* info() const;
};

}
