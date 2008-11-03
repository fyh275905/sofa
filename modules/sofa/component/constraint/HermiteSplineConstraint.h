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
#ifndef SOFA_COMPONENT_CONSTRAINT_HERMITESPLINECONSTRAINT_H
#define SOFA_COMPONENT_CONSTRAINT_HERMITESPLINECONSTRAINT_H

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

/**
	Impose a trajectory to given Dofs following a Hermite cubic spline constraint.
	Control parameters are :
	  - begin and end points
	  - derivates at this points
	  - acceleration curve on the trajectory
	*/
template <class DataTypes>
class HermiteSplineConstraint : public core::componentmodel::behavior::Constraint<DataTypes>, public virtual core::objectmodel::BaseObject
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
    typedef typename defaulttype::Vec<2, Real> Vec2R;
    typedef typename  helper::Quater<Real> QuatR;

public:
    ///indices of the DOFs constraints
    Data<SetIndex> m_indices;

    /// the time steps defining the duration of the constraint
    Data<Real> m_tBegin;
    Data<Real> m_tEnd;

    /// control parameters :
    /// first control point
    Data<Vec3R> m_x0;
    /// first derivated control point
    Data<Vec3R> m_dx0;
    /// second control point
    Data<Vec3R> m_x1;
    /// second derivated control point
    Data<Vec3R> m_dx1;
    /// acceleration parameters : the accaleration curve is itself a hermite spline, with first point at (0,0) and second at (1,1)
    /// and derivated on this points are :
    Data<Vec2R> m_sx0;
    Data<Vec2R> m_sx1;



public:
    HermiteSplineConstraint();

    HermiteSplineConstraint(core::componentmodel::behavior::MechanicalState<DataTypes>* mstate);

    ~HermiteSplineConstraint();

    void clearConstraints();
    void addConstraint(unsigned index );

    void setBeginTime(const Real &t) {m_tBegin.setValue(t);}
    void setEndTime(const Real &t) {m_tEnd.setValue(t);}

    Real getBeginTime() {return m_tBegin.getValue();}
    Real getEndTime() {return m_tEnd.getValue();}

    void computeHermiteCoefs( const Real u, Real &H00, Real &H10, Real &H01, Real &H11);
    void computeDerivateHermiteCoefs( const Real u, Real &dH00, Real &dH10, Real &dH01, Real &dH11);

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
