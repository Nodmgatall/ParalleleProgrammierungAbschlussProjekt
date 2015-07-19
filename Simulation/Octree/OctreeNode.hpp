/*
 * A lot of this was found here: https://github.com/brandonpelfrey/SimpleOctree/
 * With modifications by ourselves.
 */

#ifndef OCTREE_NODE_HPP
#define OCTREE_NODE_HPP

#include <vector>
#include "../Util/vec3.hpp"

class OctreeNode
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
        OctreeNode *children[8];

        // TODO: might replace with own datatype
        /** Data stored in this node (NULL if it's an interior node) */
        void * data;

    public:
        /** Default constructor */
        OctreeNode();

        /** Constructor, given an origin and radii */
        OctreeNode(Vec3<double>& origin, Vec3<double>& radii);

        /** Copy-constructor */
        OctreeNode(const OctreeNode& copy);

        /** Destructor */
        ~OctreeNode();

        /** Figure out which octant would contain the given point */
        int getOctant(const Vec3<double>& point);

        /** Figure out wether this node is a leaf node or not */
        bool isLeafNode();

        /** Insert data into tree */
        void insert(void * data);

        // TODO: probably going to need more functions...
};

#endif //OCTREE_NODE_HPP
