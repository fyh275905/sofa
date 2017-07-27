#ifndef DISTANCE_GRID_COMPONENT_H
#define DISTANCE_GRID_COMPONENT_H

#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <SofaVolumetricData/ImplicitShape.h>
#include <SofaVolumetricData/DistanceGrid.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/loader/BaseLoader.h>
#include <sofa/core/ObjectFactory.h>


namespace sofa
{

namespace core
{

typedef sofa::component::container::DistanceGrid DistanceGrid;
typedef sofa::defaulttype::Vector3 Coord;
using namespace sofa::core::objectmodel;

class DistanceGridComponent : public ImplicitShape {

public:
    SOFA_CLASS(DistanceGridComponent, BaseObject);
    DistanceGridComponent()
        : in_filename(initData(&in_filename,"filename","filename"))
        , in_nx(initData(&in_nx,0,"nx","in_nx"))
        , in_ny(initData(&in_ny,0,"ny","in_ny"))
        , in_nz(initData(&in_nz,0,"nz","in_nz"))
    {
    }
    virtual ~DistanceGridComponent()  { }
    DistanceGrid* grid {nullptr};
    void loadGrid(double scale, double sampling, int nx, int ny, int nz, Coord pmin, Coord pmax);
    virtual void init();
    virtual double eval(Coord p);

private:
    DataFileName in_filename;
    Coord pmin, pmax;
    Data<int> in_nx;
    Data<int> in_ny;
    Data<int> in_nz;

};


}

}

#endif
