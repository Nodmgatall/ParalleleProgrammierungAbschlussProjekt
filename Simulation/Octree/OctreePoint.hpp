#ifndef OCTREE_POINT_HPP
#define OCTREE_POINT_HPP

#include "../Util/vec3.hpp"

class OctreePoint {
    private:
        Vec3<double> position;
        double radius;
    public:
        OctreePoint() {/**/}
        OctreePoint(const Vec3<double>& position, double radius) : position(position), radius(radius) {/**/}
        inline const Vec3<double>& getPosition() const {return position;}
        inline void setPosition(const Vec3<double>& p) {position = p;}
        inline double getRadius() {return radius;}
        inline void setRadius(double r) {radius = r;}
};


#endif // OCTREE_POINT_HPP
