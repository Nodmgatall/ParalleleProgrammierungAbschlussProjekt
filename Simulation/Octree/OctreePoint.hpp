#ifndef OCTREE_POINT_HPP
#define OCTREE_POINT_HPP

#include "../Util/vec3.hpp"

class OctreePoint {
    private:
        Vec3<double> position;
        double radius;
        unsigned long index;
    public:
        OctreePoint() {/**/}
        OctreePoint(const Vec3<double>& position, const double radius, const unsigned long index) : position(position), radius(radius), index(index) {/**/}
        inline Vec3<double>& getPosition() {return position;}
        inline void setPosition(const Vec3<double>& p) {position = p;}
        inline double getRadius() {return radius;}
        inline void setRadius(double r) {radius = r;}
        inline unsigned long getIndex() {return index;}
        inline void setIndex(unsigned long i) {index = i;}
};


#endif // OCTREE_POINT_HPP
