/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_COLLISION_POINTMODEL_H
#define SOFA_COMPONENT_COLLISION_POINTMODEL_H

#include <sofa/core/CollisionModel.h>
#include <sofa/component/container/MechanicalObject.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

class PointModel;

class Point : public core::TCollisionElementIterator<PointModel>
{
public:
    Point(PointModel* model, int index);

    explicit Point(core::CollisionElementIterator& i);

    const Vector3& p() const;
    const Vector3& pFree() const;
    const Vector3& v() const;
    Vector3 n() const;

    bool testLMD(const Vector3 &, double &, double &);
};

class PointModel : public core::CollisionModel
{
public:
    typedef Vec3Types InDataTypes;
    typedef Vec3Types DataTypes;
    typedef DataTypes::VecCoord VecCoord;
    typedef DataTypes::VecDeriv VecDeriv;
    typedef DataTypes::Coord Coord;
    typedef DataTypes::Deriv Deriv;
    typedef Point Element;
    friend class Point;

    PointModel();

    virtual void init();

    // -- CollisionModel interface

    virtual void resize(int size);

    virtual void computeBoundingTree(int maxDepth=0);

    virtual void computeContinuousBoundingTree(double dt, int maxDepth=0);

    void draw(int index);

    void draw();

    virtual bool canCollideWithElement(int index, CollisionModel* model2, int index2);

    core::componentmodel::behavior::MechanicalState<Vec3Types>* getMechanicalState() { return mstate; }

    //virtual const char* getTypeName() const { return "Point"; }



protected:

    core::componentmodel::behavior::MechanicalState<Vec3Types>* mstate;

    Data<bool> computeNormals;

    VecDeriv normals;

    void updateNormals();
};

inline Point::Point(PointModel* model, int index)
    : core::TCollisionElementIterator<PointModel>(model, index)
{}

inline Point::Point(core::CollisionElementIterator& i)
    : core::TCollisionElementIterator<PointModel>(static_cast<PointModel*>(i.getCollisionModel()), i.getIndex())
{
}

inline const Vector3& Point::p() const { return (*model->mstate->getX())[index]; }

inline const Vector3& Point::pFree() const { return (*model->mstate->getXfree())[index]; }

inline const Vector3& Point::v() const { return (*model->mstate->getV())[index]; }

inline Vector3 Point::n() const { return ((unsigned)index<model->normals.size()) ? model->normals[index] : Vector3(); }

//bool Point::testLMD(const Vector3 &PQ, double &coneFactor, double &coneExtension);

} // namespace collision

} // namespace component

} // namespace sofa

#endif
