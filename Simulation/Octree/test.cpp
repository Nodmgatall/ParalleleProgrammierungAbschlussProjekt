#include <iostream>
#include <assert.h>

#include "Octree.hpp"

void test1()
{
    Octree* mytree = new Octree();
    std::cout << mytree->getOrigin().getX() << std::endl;
    std::cout << mytree->getRadii().getX() << std::endl;
    delete mytree;
}

void test2()
{
    Vec3<double> myorigin = Vec3<double>(0.0, 0.0, 0.0);
    Vec3<double> myradii = Vec3<double>(10.0, 10.0, 10.0);

    Octree* mytree = new Octree(myorigin, myradii);

    assert(mytree->getOrigin().getX() == 0.0);
    assert(mytree->getRadii().getX() == 10.0);

    double i = 5.0;

    OctreePoint *mydata = new OctreePoint(Vec3<double>(43.0, 21.0, 24.0), i, 7);

    mytree->insert(mydata);

    std::vector<OctreePoint*> results;
    mytree->getPointsInBox(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(45.0, 25.0, 25.0), results);
    std::cout << results[0]->getIndex() << std::endl;
}

int main(void)
{
    test1();
    test2();
    std::cout << "Tests completed successfully!" << std::endl;
    return 0;
}
