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
        OctreePoint(const Vec3<double>& position, double radius, unsigned long index) : position(position), radius(radius), index(index) {/**/}
        inline const Vec3<double>& getPosition() const {return position;}
        inline void setPosition(const Vec3<double>& p) {position = p;}
        inline double getRadius() {return radius;}
        inline void setRadius(double r) {radius = r;}
};


#endif // OCTREE_POINT_HPP
