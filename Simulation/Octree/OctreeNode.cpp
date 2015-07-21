#include "OctreeNode.hpp"

OctreeNode::OctreeNode()
{
    int i;
    origin = Vec3<double>();
    radii = Vec3<double>();

    for (i = 0; i < 8; i++) {
        children[i] = NULL;
    }
}

OctreeNode::OctreeNode(Vec3<double>& origin, Vec3<double>& radii)
    :origin(origin), radii(radii)
{
    int i;
    data = NULL;

    for (i = 0; i < 8; i++) {
        children[i] = NULL;
    }
}

OctreeNode::OctreeNode(const OctreeNode& copy)
    :origin(copy.origin), radii(copy.radii), data(copy.data)
{
    
}

OctreeNode::~OctreeNode()
{
    int i;

    for (i = 0; i < 8; i++)
        delete children[i];
}
