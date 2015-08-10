/*
 * A lot of this was found here: https://github.com/brandonpelfrey/SimpleOctree/
 * With modifications by ourselves.
 */

#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <vector>
#include "../Util/vec3.hpp"
#include "OctreePoint.hpp"

class Octree
{
    private:
        // TODO: maybe use ints instead of doubles for all positions?
        /** Physical center */
        Vec3<double> origin;

        // TODO: could be replaced by just one value if our space is always
        // spherical or cubic
        /** The radii (in x, y and z direction respectively) of this node */
        Vec3<double> radii;

        /** Pointers to this node's children */
        Octree *children[8];

        /** Data stored in this node (NULL if it's an interior node) */
        OctreePoint *data;

    public:
        /** Default constructor */
        Octree();

        /** Constructor, given an origin and radii */
        Octree(const Vec3<double>& origin, const Vec3<double>& radii);

        /** Copy-constructor */
        Octree(const Octree& copy);

        /** Destructor */
        ~Octree();

        /** Figure out which octant would contain the given point */
        int getOctant(const Vec3<double>& point);

        /** Figure out wether this node is a leaf node or not */
        bool isLeafNode();

        /** Insert data into tree */
        void insert(OctreePoint* point);

        /** Return node's origin */
        Vec3<double> getOrigin();

        /** Return node's radii */
        Vec3<double> getRadii();

        /** Get stored data */
        const void * getData();

        /** Get all points within a box, delimited by
            vectors bmin and bmax. Results are pushed into 'results' */
        void getPointsInBox(const Vec3<double>& bmin, const Vec3<double>& bmax,
                            std::vector<OctreePoint*>& results);

        /** Get child by index (0-7) */
        Octree * getChild(int num);

        // TODO: probably going to need more functions...
};

#endif //OCTREE_HPP
