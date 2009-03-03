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
#ifndef SOFA_COMPONENT_FORCEFIELD_REGULARGRIDSPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_REGULARGRIDSPRINGFORCEFIELD_H

#include <sofa/component/forcefield/StiffSpringForceField.h>
#include <sofa/component/topology/RegularGridTopology.h>
#ifdef SOFA_DEV
#include <sofa/component/topology/FittedRegularGridTopology.h>
#endif // SOFA_DEV

namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class RegularGridSpringForceField : public StiffSpringForceField<DataTypes>
{
    double m_potentialEnergy;
public:
    typedef StiffSpringForceField<DataTypes> Inherit;
    typedef typename Inherit::Spring Spring;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

    //virtual const char* getTypeName() const { return "RegularGridSpringForceField"; }

protected:
    Data< Real > linesStiffness;
    Data< Real > linesDamping;
    Data< Real > quadsStiffness;
    Data< Real > quadsDamping;
    Data< Real > cubesStiffness;
    Data< Real > cubesDamping;

public:
    RegularGridSpringForceField(core::componentmodel::behavior::MechanicalState<DataTypes>* object1, core::componentmodel::behavior::MechanicalState<DataTypes>* object2)
        : StiffSpringForceField<DataTypes>(object1, object2),
          linesStiffness  (initData(&linesStiffness,Real(100),"linesStiffness","Lines Stiffness"))
          , linesDamping  (initData(&linesDamping  ,Real(5),"linesDamping"  ,"Lines Damping"))
          , quadsStiffness(initData(&quadsStiffness,Real(100),"quadsStiffness","Quads Stiffness"))
          , quadsDamping  (initData(&quadsDamping  ,Real(5),"quadsDamping"  ,"Quads Damping"))
          , cubesStiffness(initData(&cubesStiffness,Real(100),"cubesStiffness","Cubes Stiffness"))
          , cubesDamping  (initData(&cubesDamping  ,Real(5),"cubesDamping"  ,"Cubes Damping"))
          , topology(NULL)
#ifdef SOFA_DEV
          , trimmedTopology(NULL)
#endif // SOFA_DEV
    {
        addAlias(&linesStiffness,    "stiffness"); addAlias(&linesDamping,    "damping");
        addAlias(&quadsStiffness,    "stiffness"); addAlias(&quadsDamping,    "damping");
        addAlias(&cubesStiffness,    "stiffness"); addAlias(&cubesDamping,    "damping");
    }

    RegularGridSpringForceField()
        :
        linesStiffness  (initData(&linesStiffness,Real(100),"linesStiffness","Lines Stiffness"))
        , linesDamping  (initData(&linesDamping  ,Real(5),"linesDamping"  ,"Lines Damping"))
        , quadsStiffness(initData(&quadsStiffness,Real(100),"quadsStiffness","Quads Stiffness"))
        , quadsDamping  (initData(&quadsDamping  ,Real(5),"quadsDamping"  ,"Quads Damping"))
        , cubesStiffness(initData(&cubesStiffness,Real(100),"cubesStiffness","Cubes Stiffness"))
        , cubesDamping  (initData(&cubesDamping  ,Real(5),"cubesDamping"  ,"Cubes Damping"))
        , topology(NULL)
#ifdef SOFA_DEV
        , trimmedTopology(NULL)
#endif // SOFA_DEV
    {
        addAlias(&linesStiffness,    "stiffness"); addAlias(&linesDamping,    "damping");
        addAlias(&quadsStiffness,    "stiffness"); addAlias(&quadsDamping,    "damping");
        addAlias(&cubesStiffness,    "stiffness"); addAlias(&cubesDamping,    "damping");
    }

    Real getStiffness() const { return linesStiffness.getValue(); }
    Real getLinesStiffness() const { return linesStiffness.getValue(); }
    Real getQuadsStiffness() const { return quadsStiffness.getValue(); }
    Real getCubesStiffness() const { return cubesStiffness.getValue(); }
    void setStiffness(Real val)
    {
        linesStiffness.setValue(val);
        quadsStiffness.setValue(val);
        cubesStiffness.setValue(val);
    }
    void setLinesStiffness(Real val)
    {
        linesStiffness.setValue(val);
    }
    void setQuadsStiffness(Real val)
    {
        quadsStiffness.setValue(val);
    }
    void setCubesStiffness(Real val)
    {
        cubesStiffness.setValue(val);
    }

    Real getDamping() const { return linesDamping.getValue(); }
    Real getLinesDamping() const { return linesDamping.getValue(); }
    Real getQuadsDamping() const { return quadsDamping.getValue(); }
    Real getCubesDamping() const { return cubesDamping.getValue(); }
    void setDamping(Real val)
    {
        linesDamping.setValue(val);
        quadsDamping.setValue(val);
        cubesDamping.setValue(val);
    }
    void setLinesDamping(Real val)
    {
        linesDamping.setValue(val);
    }
    void setQuadsDamping(Real val)
    {
        quadsDamping.setValue(val);
    }
    void setCubesDamping(Real val)
    {
        cubesDamping.setValue(val);
    }

    virtual void init();

    virtual void addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2);

    virtual void addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2, double kFactor, double bFactor);

    virtual void draw();

protected:
    topology::RegularGridTopology* topology;
#ifdef SOFA_DEV
    topology::FittedRegularGridTopology* trimmedTopology;
#endif // SOFA_DEV
};
#if defined(WIN32) && !defined(SOFA_COMPONENT_FORCEFIELD_REGULARGRIDSPRINGFORCEFIELD_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec3dTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec2dTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec1dTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec6dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec3fTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec2fTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec1fTypes>;
extern template class SOFA_COMPONENT_FORCEFIELD_API RegularGridSpringForceField<defaulttype::Vec6fTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
