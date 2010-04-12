/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_ENGINE_BOXROI_H
#define SOFA_COMPONENT_ENGINE_BOXROI_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/defaulttype/Vec.h>
#include <sofa/core/DataEngine.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/component/topology/PointSubset.h>

namespace sofa
{

namespace component
{

namespace engine
{

using namespace core::componentmodel::behavior;
using namespace core::objectmodel;

/**
 * This class find all the points located inside a given box.
 */
template <class DataTypes>
class BoxROI : public core::DataEngine
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(BoxROI,DataTypes),core::DataEngine);
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Real Real;
    typedef defaulttype::Vec<6,Real> Vec6;
    typedef topology::PointSubset SetIndex;
    typedef typename DataTypes::CPos CPos;

    typedef defaulttype::Vec<3,Real> Point;
    typedef unsigned int PointID;
    typedef core::componentmodel::topology::BaseMeshTopology::Edge Edge;
    typedef core::componentmodel::topology::BaseMeshTopology::Triangle Triangle;
    typedef core::componentmodel::topology::BaseMeshTopology::Tetra Tetra;

protected:
    bool isPointInBox(const CPos& p, const Vec6& b);
    bool isPointInBox(const PointID& pid, const Vec6& b);
    bool isEdgeInBox(const Edge& e, const Vec6& b);
    bool isTriangleInBox(const Triangle& t, const Vec6& b);
    bool isTetrahedronInBox(const Tetra& t, const Vec6& b);
public:

    BoxROI();

    ~BoxROI() {}

    void init();

    void reinit();

    void update();

    void draw();

    bool addBBox(double* minBBox, double* maxBBox);

    /// this constraint is holonomic
    bool isHolonomic() {return true;}

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (dynamic_cast<MechanicalState<DataTypes>*>(context->getMechanicalState()) == NULL)
            return false;
        return BaseObject::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::objectmodel::BaseObject::create(obj, context, arg);
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const BoxROI<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

    //Input
    Data< helper::vector<Vec6> > boxes;
    Data<VecCoord> f_X0;
    Data<helper::vector<Edge> > f_edges;
    Data<helper::vector<Triangle> > f_triangles;
    Data<helper::vector<Tetra> > f_tetrahedra;

    //Output
    Data<SetIndex> f_indices;
    Data<SetIndex> f_edgeIndices;
    Data<SetIndex> f_triangleIndices;
    Data<SetIndex> f_tetrahedronIndices;
    Data<VecCoord > f_pointsInBox;
    Data<VecCoord > f_pointsOutBox;
    Data<helper::vector<Edge> > f_edgesInBox;
    Data<helper::vector<Triangle> > f_trianglesInBox;
    Data<helper::vector<Triangle> > f_trianglesOutBox;
    Data<helper::vector<Tetra> > f_tetrahedraInBox;
    Data<helper::vector<Tetra> > f_tetrahedraOutBox;

    //Parameter
    Data<bool> p_subsetTopology;
    Data<bool> p_drawBoxes;
    Data<bool> p_drawPoints;
    Data<bool> p_drawEdges;
    Data<bool> p_drawTriangles;
    Data<bool> p_drawTetrahedra;
};

#if defined(WIN32) && !defined(SOFA_COMPONENT_ENGINE_BOXROI_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_ENGINE_API BoxROI<defaulttype::Vec3dTypes>;
#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_ENGINE_API BoxROI<defaulttype::Vec3fTypes>;
#endif //SOFA_DOUBLE
#endif

} // namespace engine

} // namespace component

} // namespace sofa

#endif
