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
#ifndef SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_H
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/Data.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/component/component.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class PlaneForceFieldInternalData
{
public:
};

template<class DataTypes>
class PlaneForceField : public core::componentmodel::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(PlaneForceField, DataTypes), SOFA_TEMPLATE(core::componentmodel::behavior::ForceField, DataTypes));
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

protected:
    sofa::helper::vector<unsigned int> contacts;

    PlaneForceFieldInternalData<DataTypes> data;

public:

    Data<Deriv> planeNormal;
    Data<Real> planeD;
    Data<Real> stiffness;
    Data<Real> damping;
    Data<defaulttype::Vec3f> color;
    Data<bool> bDraw;
    Data<Real> drawSize;

    /// optional range of local DOF indices. Any computation involving only indices outside of this range are discarded (useful for parallelization using mesh partitionning)
    Data< defaulttype::Vec<2,int> > localRange;

    PlaneForceField()
        : planeNormal(initData(&planeNormal, "normal", "plane normal"))
        , planeD(initData(&planeD, (Real)0, "d", "plane d coef"))
        , stiffness(initData(&stiffness, (Real)500, "stiffness", "force stiffness"))
        , damping(initData(&damping, (Real)5, "damping", "force damping"))
        , color(initData(&color, defaulttype::Vec3f(0.0f,.5f,.2f), "color", "plane color"))
        , bDraw(initData(&bDraw, false, "draw", "enable/disable drawing of plane"))
        , drawSize(initData(&drawSize, (Real)10.0f, "drawSize", "plane display size if draw is enabled"))
        , localRange( initData(&localRange, defaulttype::Vec<2,int>(-1,-1), "localRange", "optional range of local DOF indices. Any computation involving only indices outside of this range are discarded (useful for parallelization using mesh partitionning)" ) )
    {
        Deriv n;
        DataTypes::set(n, 0, 1, 0);
        planeNormal.setValue(n);
    }

    virtual bool canPrefetch() const { return false; }

    void setPlane(const Deriv& normal, Real d)
    {
        Real n = normal.norm();
        planeNormal.setValue( normal / n);
        planeD.setValue( d / n );
    }

    void setMState(  core::componentmodel::behavior::MechanicalState<DataTypes>* mstate ) { this->mstate = mstate; }

    void setStiffness(Real stiff)
    {
        stiffness.setValue( stiff );
    }

    void setDamping(Real damp)
    {
        damping.setValue( damp );
    }

    void rotate( Deriv axe, Real angle ); // around the origin (0,0,0)

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx, double kFactor, double bFactor);

    virtual double getPotentialEnergy(const VecCoord& x) const;

    virtual void updateStiffness( const VecCoord& x );

    virtual void addKToMatrix(sofa::defaulttype::BaseMatrix *, SReal, unsigned int &);

    void draw();
    void drawPlane(float size=0.0f);
    bool addBBox(double* minBBox, double* maxBBox);

};

#if defined(WIN32) && !defined(SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API PlaneForceField<defaulttype::Vec3dTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API PlaneForceField<defaulttype::Vec2dTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API PlaneForceField<defaulttype::Vec1dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API PlaneForceField<defaulttype::Vec3fTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API PlaneForceField<defaulttype::Vec2fTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API PlaneForceField<defaulttype::Vec1fTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
