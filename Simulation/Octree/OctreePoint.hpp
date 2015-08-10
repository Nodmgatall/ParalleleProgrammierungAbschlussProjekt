#ifndef OCTREE_POINT_HPP
#define OCTREE_POINT_HPP

#include "../Util/vec3.hpp"

class OctreePoint {
    private:
        Vec3<double> position;
        void * data;
    public:
        OctreePoint() {/**/}
        OctreePoint(const Vec3<double>& position, void * data) : position(position), data(data) {/**/}
        inline const Vec3<double>& getPosition() const {return position;}
        inline void setPosition(const Vec3<double>& p) {position = p;}
        inline const void * getData() const {return &data;}
        inline void setData(void * d) {data = d;}
};


#endif // OCTREE_POINT_HPP
