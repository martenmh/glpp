#ifndef GLPP_COORDINATES_H
#define GLPP_COORDINATES_H

#include "ml/ml.h"
#include "gui/window.h"
//#include <math-lib2/include/ml/vector.h>

using CoordinateTransformator = ml::vec2T<double> (*)(const ml::vec2T<double>& xy);

struct CoordinateSystemBase {
    double xMin = 0, xMax;
    double yMin = 0, yMax;
    bool invertX = false,
        invertY = false;
};

CoordinateTransformator toPolar(double x, double y){

}

class CoordinateMapper {
    CoordinateTransformator existingTransformation;

  public:
    ml::vec2T<double> topLeft2;
    ml::vec2T<double> absoluteBottomRight1;
    ml::vec2T<double> absoluteBottomRight2;
    CoordinateMapper(CoordinateSystemBase system1, CoordinateSystemBase system2,
					 CoordinateTransformator existingTransformation = nullptr):
        existingTransformation(existingTransformation){
        auto topLeft1 = ml::vec2T<double>{system1.invertX ? system1.xMax : system1.xMin,
                                     system1.invertY ? system1.yMin : system1.yMax};
        topLeft2 = ml::vec2T<double>{system2.invertX ? system2.xMax : system2.xMin,
                                     system2.invertY ? system2.yMin : system2.yMax};

        auto bottomRight1 = ml::vec2T<double>{system1.invertX ? system1.xMin : system1.xMax,
                                         system1.invertY ? system1.yMax : system1.yMin};
        auto bottomRight2 = ml::vec2T<double>{system2.invertX ? system2.xMin : system2.xMax,
                                         system2.invertY ? system2.yMax : system2.yMin};

        absoluteBottomRight1 = bottomRight1 - topLeft1;
        absoluteBottomRight2 = bottomRight2 - topLeft2;
    }

    ml::vec2T<double> operator()(const ml::vec2T<double>& xy){

        auto relativeCoordinates = xy / absoluteBottomRight1;
        auto absoluteCoordinates2 = relativeCoordinates * absoluteBottomRight2 + topLeft2;
        if(existingTransformation){
            return existingTransformation(absoluteCoordinates2);
        }
        return absoluteCoordinates2;
    }
};

CoordinateSystemBase createWindowCoordinates(const glpp::Window& window) {
    return CoordinateSystemBase{
        .xMax    = static_cast<double>(window.width()),
        .yMax    = static_cast<double>(window.height()),
        .invertY = true
    };
}


#endif //GLPP_COORDINATES_H
