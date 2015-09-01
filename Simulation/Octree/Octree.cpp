#include "Octree.hpp"

Octree::Octree()
{
    int i;
    origin = Vec3<double>();
    radii = Vec3<double>();
    data = NULL;

    for (i = 0; i < 8; i++) {
        children[i] = NULL;
    }
}

Octree::Octree(const Vec3<double>& origin, const Vec3<double>& radii)
    :origin(origin), radii(radii)
{
    int i;
    data = NULL;

    for (i = 0; i < 8; i++) {
        children[i] = NULL;
    }
}

Octree::Octree(const Octree& copy)
    :origin(copy.origin), radii(copy.radii), data(copy.data)
{
    
}

Octree::~Octree()
{
    int i;

    for (i = 0; i < 8; i++)
        delete children[i];
}

int Octree::getOctant(Vec3<double>& point)
{
    int oct = 0;
    //Vec3<double> cpypoint = point;
    // clever way of definitively returning the correct
    // number between 0 and 7.
    if(point.getX() >= origin.getX()) oct |= 4;
    if(point.getY() >= origin.getY()) oct |= 2;
    if(point.getZ() >= origin.getZ()) oct |= 1;
    return oct;
}

bool Octree::isLeafNode()
{
    // we are a leaf if we have no children
    // we have no children if our children-array
    // is full of NULLs.
    return children[0] == NULL;
}

void Octree::insert(OctreePoint* newdata)
{
    int i;

    if (isLeafNode()) {
        if (data == NULL) {
            // we're at a leaf that doesn't store any data.
            // thus, we can simply save the data and return.
            data = newdata;
            return;
        } else {
            // we're at a leaf, but we already store data.
            // thus, we're splitting this node into 8 new ones.
            OctreePoint * olddata = data;
            data = NULL;

            for (i = 0; i < 8; ++i) {
                Vec3<double> newOrigin = origin;

                newOrigin.setX(origin.getX() + radii.getX() * (i&4 ? .5f : -.5f));
                newOrigin.setY(origin.getY() + radii.getY() * (i&2 ? .5f : -.5f));
                newOrigin.setZ(origin.getZ() + radii.getZ() * (i&1 ? .5f : -.5f));
                children[i] = new Octree(newOrigin, radii * .5f);
            }

            // re-insert
            children[getOctant(olddata->getPosition())]->insert(olddata);
            children[getOctant(newdata->getPosition())]->insert(newdata);
        }
    } else {
        // we're not at a leaf, thus we try to insert the data into
        // one of our children.
        // This comment could have been phrased a lot more sensibly. Oh well.
        i = getOctant(newdata->getPosition());
        children[i]->insert(newdata);
    }
}

Vec3<double> Octree::getOrigin()
{
    return origin;
}

Vec3<double> Octree::getRadii()
{
    return radii;
}

void Octree::setRadii(Vec3<double> r)
{
    radii = r;
}

void Octree::getPointsInBox(const Vec3<double>& bmin, const Vec3<double>& bmax,
                    std::vector<OctreePoint*>& results)
{
    if (isLeafNode()) {
        if (data != NULL) {
            Vec3<double> p = data->getPosition();
            if (p.getX() > bmax.getX() || p.getY() > bmax.getY() || p.getZ() > bmax.getZ()) return;
            if (p.getX() < bmin.getX() || p.getY() < bmin.getY() || p.getZ() < bmin.getZ()) return;
            results.push_back(data);
        }
    } else {
        // We're at an interior node and need to check if this node is inside the bounding box
        int i;

        for (i = 0; i < 8; i++) {
            // compute min/max corners of this child octant
            Vec3<double> cmax = children[i]->getOrigin() + children[i]->getRadii();
            Vec3<double> cmin = children[i]->getOrigin() - children[i]->getRadii();

            // if the queried rectangle is outside the child's bounding box, continue
            if (cmax.getX() < bmin.getX() || cmax.getY() < bmin.getY() || cmax.getZ() < bmin.getZ()) continue;
            if (cmin.getX() > bmax.getX() || cmin.getY() > bmax.getY() || cmin.getZ() > bmax.getZ()) continue;

            // If we've gotten this far, we know that this child is intersecting the queried box
            children[i]->getPointsInBox(bmin, bmax, results);
        }
    }
}

Octree * Octree::getChild(int num) {
    if (num < 0 || num > 7)
        return NULL;
    return children[num];
}
