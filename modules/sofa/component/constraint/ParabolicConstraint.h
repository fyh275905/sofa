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
#ifndef SOFA_COMPONENT_CONSTRAINT_PARABOLICCONSTRAINT_H
#define SOFA_COMPONENT_CONSTRAINT_PARABOLICCONSTRAINT_H

#include <sofa/core/componentmodel/behavior/Constraint.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/helper/vector.h>
#include <sofa/component/topology/PointSubset.h>


namespace sofa
{

namespace component
{

namespace constraint
{

using core::objectmodel::Data;
using namespace sofa::core::objectmodel;
using namespace sofa::defaulttype;

/** Apply a parabolic trajectory to particles going through 3 points specified by the user.
	The DOFs set in the "indices" list follow the computed parabol from "tBegin" to "tEnd".
	*/
template <class DataTypes>
class ParabolicConstraint : public core::componentmodel::behavior::Constraint<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef topology::PointSubset SetIndex;
    typedef helper::vector<unsigned int> SetIndexArray;
    typedef typename defaulttype::Vec<3, Real> Vec3R;
    typedef typename  helper::Quater<Real> QuatR;

protected:
    ///indices of the DOFs constraints
    Data<SetIndex> m_indices;

    /// the three points defining the parabol
    Data<Vec3R> m_P1;
    Data<Vec3R> m_P2;
    Data<Vec3R> m_P3;

    /// the time steps defining the velocity of the movement
    Data<Real> m_tBegin;
    Data<Real> m_tEnd;

    /// the 3 points projected in the parabol plan
    Vec3R m_locP1;
    Vec3R m_locP2;
    Vec3R m_locP3;

    /// the quaternion doing the projection
    QuatR m_projection;



public:
    ParabolicConstraint();

    ParabolicConstraint(core::componentmodel::behavior::MechanicalState<DataTypes>* mstate);

    ~ParabolicConstraint();

    void addConstraint(unsigned index );

    void setP1(const Vec3R &p) {m_P1.setValue(p);}
    void setP2(const Vec3R &p) {m_P2.setValue(p);}
    void setP3(const Vec3R &p) {m_P3.setValue(p);}

    void setBeginTime(const Real &t) {m_tBegin.setValue(t);}
    void setEndTime(const Real &t) {m_tEnd.setValue(t);}

    Vec3R getP1() {return m_P1.getValue();}
    Vec3R getP2() {return m_P2.getValue();}
    Vec3R getP3() {return m_P3.getValue();}

    Real getBeginTime() {return m_tBegin.getValue();}
    Real getEndTime() {return m_tEnd.getValue();}

    /// -- Constraint interface
    void init();
    void reinit();
    void projectResponse(VecDeriv& dx);
    virtual void projectVelocity(VecDeriv& dx); ///< project dx to constrained space (dx models a velocity)
    virtual void projectPosition(VecCoord& x); ///< project x to constrained space (x models a position)

    void draw();
};

} // namespace constraint

} // namespace component

} // namespace sofa

#endif
