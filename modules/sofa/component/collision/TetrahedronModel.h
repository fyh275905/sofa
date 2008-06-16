/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_COMPONENT_COLLISION_TETRAHEDRONMODEL_H
#define SOFA_COMPONENT_COLLISION_TETRAHEDRONMODEL_H

#include <sofa/core/CollisionModel.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/component/topology/TetrahedronSetTopology.h>

#include <map>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

class TetrahedronModel;

class Tetrahedron : public core::TCollisionElementIterator<TetrahedronModel>
{
public:
    Tetrahedron(TetrahedronModel* model, int index);
    Tetrahedron() {};
    explicit Tetrahedron(core::CollisionElementIterator& i);

    const Vector3& p1() const;
    const Vector3& p2() const;
    const Vector3& p3() const;
    const Vector3& p4() const;
    int p1Index() const;
    int p2Index() const;
    int p3Index() const;
    int p4Index() const;

    const Vector3& p1Free() const;
    const Vector3& p2Free() const;
    const Vector3& p3Free() const;
    const Vector3& p4Free() const;

    const Vector3& v1() const;
    const Vector3& v2() const;
    const Vector3& v3() const;
    const Vector3& v4() const;

    Vector3 getBary(const Vector3& p) const;
    Vector3 getDBary(const Vector3& v) const;

    Vector3 getCoord(const Vector3& b) const;
    Vector3 getDCoord(const Vector3& b) const;

};

class TetrahedronModel : public core::CollisionModel
{
public:
    typedef Vec3Types InDataTypes;
    typedef Vec3Types DataTypes;
    typedef DataTypes::VecCoord VecCoord;
    typedef DataTypes::VecDeriv VecDeriv;
    typedef DataTypes::Coord Coord;
    typedef DataTypes::Deriv Deriv;
    typedef Tetrahedron Element;
    typedef topology::TetrahedronSetTopology<DataTypes> SetTopology;
    friend class Tetrahedron;

protected:
    struct TetrahedronInfo
    {
        Vector3 coord0;
        Matrix3 coord2bary;
        Matrix3 bary2coord;
    };

    sofa::helper::vector<TetrahedronInfo> elems;
    const sofa::helper::vector<topology::Tetrahedron>* tetra;

    core::componentmodel::behavior::MechanicalState<Vec3Types>* mstate;

    Topology* topology;
    SetTopology* setTopology;

public:

    TetrahedronModel();

    virtual void init();

    // -- CollisionModel interface

    virtual void resize(int size);

    virtual void computeBoundingTree(int maxDepth=0);

    //virtual void computeContinuousBoundingTree(double dt, int maxDepth=0);

    void draw(int index);

    void draw();

    virtual void handleTopologyChange();

    core::componentmodel::behavior::MechanicalState<Vec3Types>* getMechanicalState() { return mstate; }
    Topology* getTopology() { return topology; }
};

inline Tetrahedron::Tetrahedron(TetrahedronModel* model, int index)
    : core::TCollisionElementIterator<TetrahedronModel>(model, index)
{}

inline Tetrahedron::Tetrahedron(core::CollisionElementIterator& i)
    : core::TCollisionElementIterator<TetrahedronModel>(static_cast<TetrahedronModel*>(i.getCollisionModel()), i.getIndex())
{}

inline const Vector3& Tetrahedron::p1() const { return (*model->mstate->getX())[(*(model->tetra))[index][0]]; }
inline const Vector3& Tetrahedron::p2() const { return (*model->mstate->getX())[(*(model->tetra))[index][1]]; }
inline const Vector3& Tetrahedron::p3() const { return (*model->mstate->getX())[(*(model->tetra))[index][2]]; }
inline const Vector3& Tetrahedron::p4() const { return (*model->mstate->getX())[(*(model->tetra))[index][3]]; }

inline const Vector3& Tetrahedron::p1Free() const { return (*model->mstate->getXfree())[(*(model->tetra))[index][0]]; }
inline const Vector3& Tetrahedron::p2Free() const { return (*model->mstate->getXfree())[(*(model->tetra))[index][1]]; }
inline const Vector3& Tetrahedron::p3Free() const { return (*model->mstate->getXfree())[(*(model->tetra))[index][2]]; }
inline const Vector3& Tetrahedron::p4Free() const { return (*model->mstate->getXfree())[(*(model->tetra))[index][3]]; }

inline int Tetrahedron::p1Index() const { return (*(model->tetra))[index][0]; }
inline int Tetrahedron::p2Index() const { return (*(model->tetra))[index][1]; }
inline int Tetrahedron::p3Index() const { return (*(model->tetra))[index][2]; }
inline int Tetrahedron::p4Index() const { return (*(model->tetra))[index][3]; }

inline const Vector3& Tetrahedron::v1() const { return (*model->mstate->getV())[(*(model->tetra))[index][0]]; }
inline const Vector3& Tetrahedron::v2() const { return (*model->mstate->getV())[(*(model->tetra))[index][1]]; }
inline const Vector3& Tetrahedron::v3() const { return (*model->mstate->getV())[(*(model->tetra))[index][2]]; }
inline const Vector3& Tetrahedron::v4() const { return (*model->mstate->getV())[(*(model->tetra))[index][3]]; }

inline Vector3 Tetrahedron::getBary(const Vector3& p) const { return model->elems[index].coord2bary*(p-model->elems[index].coord0); }
inline Vector3 Tetrahedron::getDBary(const Vector3& v) const { return model->elems[index].coord2bary*(v); }
inline Vector3 Tetrahedron::getCoord(const Vector3& b) const { return model->elems[index].bary2coord*b + model->elems[index].coord0; }
inline Vector3 Tetrahedron::getDCoord(const Vector3& b) const { return model->elems[index].bary2coord*b; }

} // namespace collision

} // namespace component

} // namespace sofa

#endif
