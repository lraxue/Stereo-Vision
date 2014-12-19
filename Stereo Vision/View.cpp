//
//  View.cpp
//  Stereo Vision
//
//  Created by Neo on 14-12-3.
//  Copyright (c) 2014年 PKU. All rights reserved.
//

#include "View.h"

namespace sv {
    /* Public interfaces */
    void View::extractFeaturePoints() {
        cv::SiftFeatureDetector detector;
        cv::SiftDescriptorExtractor extractor;
        detector.detect(mImg, mFeaturePoints);
        extractor.compute(mImg, mFeaturePoints, mDescriptor);
        std::cout << "Feature Points:" << mFeaturePoints.size() << std::endl;
    }
}