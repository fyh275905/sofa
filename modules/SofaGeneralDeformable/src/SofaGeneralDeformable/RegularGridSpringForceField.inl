/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <SofaGeneralDeformable/RegularGridSpringForceField.h>
#include <sofa/core/visual/VisualParams.h>

namespace sofa::component::interactionforcefield
{

template<class DataTypes>
RegularGridSpringForceField<DataTypes>::RegularGridSpringForceField()
    : RegularGridSpringForceField(nullptr)
{
}

template<class DataTypes>
RegularGridSpringForceField<DataTypes>::RegularGridSpringForceField(core::behavior::MechanicalState<DataTypes>* mstate)
    : StiffSpringForceField<DataTypes>(mstate),
      linesStiffness  (initData(&linesStiffness,Real(100),"linesStiffness","Lines Stiffness"))
      , linesDamping  (initData(&linesDamping  ,Real(5),"linesDamping"  ,"Lines Damping"))
      , quadsStiffness(initData(&quadsStiffness,Real(100),"quadsStiffness","Quads Stiffness"))
      , quadsDamping  (initData(&quadsDamping  ,Real(5),"quadsDamping"  ,"Quads Damping"))
      , cubesStiffness(initData(&cubesStiffness,Real(100),"cubesStiffness","Cubes Stiffness"))
      , cubesDamping  (initData(&cubesDamping  ,Real(5),"cubesDamping"  ,"Cubes Damping"))
      , topology(nullptr)
{
    this->addAlias(&linesStiffness,    "stiffness"); this->addAlias(&linesDamping,    "damping");
    this->addAlias(&quadsStiffness,    "stiffness"); this->addAlias(&quadsDamping,    "damping");
    this->addAlias(&cubesStiffness,    "stiffness"); this->addAlias(&cubesDamping,    "damping");
}

template<class DataTypes>
void RegularGridSpringForceField<DataTypes>::init()
{
    this->StiffSpringForceField<DataTypes>::init();
    if (this->getMState())
    {
        topology = dynamic_cast<topology::RegularGridTopology*>(this->getMState()->getContext()->getMeshTopology());
    }
}

template<class DataTypes>
void RegularGridSpringForceField<DataTypes>::addForce(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v )
//addForce(VecDeriv& vf1, VecDeriv& vf2, const VecCoord& vx1, const VecCoord& vx2, const VecDeriv& vv1, const VecDeriv& vv2)
{
    // Calc any custom springs
    Inherit1::addForce(mparams, f, x, v);
    // Compute topological springs

    VecDeriv& _f = *sofa::helper::getWriteAccessor(f);
    auto _x = sofa::helper::getReadAccessor(x);
    auto _v = sofa::helper::getReadAccessor(v);

    _f.resize(_x.size());

    this->m_potentialEnergy = 0;
    const type::vector<Spring>& springs = this->springs.getValue();
    if (topology != nullptr)
    {
        const int nx = topology->getNx();
        const int ny = topology->getNy();
        const int nz = topology->getNz();
        int index = springs.size();
        int size = index;
        if (this->linesStiffness.getValue() != 0.0 || this->linesDamping.getValue() != 0.0)
            size += ((nx-1)*ny*nz+nx*(ny-1)*nz+nx*ny*(nz-1));
        if (this->quadsStiffness.getValue() != 0.0 || this->quadsDamping.getValue() != 0.0)
            size += ((nx-1)*(ny-1)*nz+(nx-1)*ny*(nz-1)+nx*(ny-1)*(nz-1))*2;
        if (this->cubesStiffness.getValue() != 0.0 || this->cubesDamping.getValue() != 0.0)
            size += ((nx-1)*(ny-1)*(nz-1))*4;
        this->dfdx.resize(size);
        if (this->linesStiffness.getValue() != 0.0 || this->linesDamping.getValue() != 0.0)
        {
            Spring spring;
            // lines along X
            spring.initpos = (Real)topology->getDx().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x+1,y,z);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring);
                    }
            // lines along Y
            spring.initpos = (Real)topology->getDy().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x,y+1,z);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring);
                    }
            // lines along Z
            spring.initpos = (Real)topology->getDz().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x,y,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring);
                    }

        }
        if (this->quadsStiffness.getValue() != 0.0 || this->quadsDamping.getValue() != 0.0)
        {
            typename RegularGridSpringForceField<DataTypes>::Spring spring1;
            typename RegularGridSpringForceField<DataTypes>::Spring spring2;
            // quads along XY plane
            // lines (x,y,z) -> (x+1,y+1,z)
            spring1.initpos = (Real)(topology->getDx()+topology->getDy()).norm();
            spring1.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring1.kd = this->quadsDamping.getValue() / spring1.initpos;
            // lines (x+1,y,z) -> (x,y+1,z)
            spring2.initpos = (Real)(topology->getDx()-topology->getDy()).norm();
            spring2.ks = this->quadsStiffness.getValue() / spring2.initpos;
            spring2.kd = this->quadsDamping.getValue() / spring2.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x+1,y+1,z);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring1);
                        spring2.m1 = topology->point(x+1,y,z);
                        spring2.m2 = topology->point(x,y+1,z);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring2);
                    }
            // quads along XZ plane
            // lines (x,y,z) -> (x+1,y,z+1)
            spring1.initpos = (Real)(topology->getDx()+topology->getDz()).norm();
            spring1.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring1.kd = this->quadsDamping.getValue() / spring1.initpos;
            // lines (x+1,y,z) -> (x,y,z+1)
            spring2.initpos = (Real)(topology->getDx()-topology->getDz()).norm();
            spring2.ks = this->quadsStiffness.getValue() / spring2.initpos;
            spring2.kd = this->quadsDamping.getValue() / spring2.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x+1,y,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring1);
                        spring2.m1 = topology->point(x+1,y,z);
                        spring2.m2 = topology->point(x,y,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring2);
                    }
            // quads along YZ plane
            // lines (x,y,z) -> (x,y+1,z+1)
            spring1.initpos = (Real)(topology->getDy()+topology->getDz()).norm();
            spring1.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring1.kd = this->quadsDamping.getValue() / spring1.initpos;
            // lines (x,y+1,z) -> (x,y,z+1)
            spring2.initpos = (Real)(topology->getDy()-topology->getDz()).norm();
            spring2.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring2.kd = this->quadsDamping.getValue() / spring1.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x,y+1,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring1);
                        spring2.m1 = topology->point(x,y+1,z);
                        spring2.m2 = topology->point(x,y,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring2);
                    }
        }
        if (this->cubesStiffness.getValue() != 0.0 || this->cubesDamping.getValue() != 0.0)
        {
            typename RegularGridSpringForceField<DataTypes>::Spring spring1;
            typename RegularGridSpringForceField<DataTypes>::Spring spring2;
            typename RegularGridSpringForceField<DataTypes>::Spring spring3;
            typename RegularGridSpringForceField<DataTypes>::Spring spring4;
            // lines (x,y,z) -> (x+1,y+1,z+1)
            spring1.initpos = (Real)(topology->getDx()+topology->getDy()+topology->getDz()).norm();
            spring1.ks = this->cubesStiffness.getValue() / spring1.initpos;
            spring1.kd = this->cubesDamping.getValue() / spring1.initpos;
            // lines (x+1,y,z) -> (x,y+1,z+1)
            spring2.initpos = (Real)(-topology->getDx()+topology->getDy()+topology->getDz()).norm();
            spring2.ks = this->cubesStiffness.getValue() / spring2.initpos;
            spring2.kd = this->cubesDamping.getValue() / spring2.initpos;
            // lines (x,y+1,z) -> (x+1,y,z+1)
            spring3.initpos = (Real)(topology->getDx()-topology->getDy()+topology->getDz()).norm();
            spring3.ks = this->cubesStiffness.getValue() / spring3.initpos;
            spring3.kd = this->cubesDamping.getValue() / spring3.initpos;
            // lines (x,y,z+1) -> (x+1,y+1,z)
            spring4.initpos = (Real)(topology->getDx()+topology->getDy()-topology->getDz()).norm();
            spring4.ks = this->cubesStiffness.getValue() / spring4.initpos;
            spring4.kd = this->cubesDamping.getValue() / spring4.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x+1,y+1,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring1);
                        spring2.m1 = topology->point(x+1,y,z);
                        spring2.m2 = topology->point(x,y+1,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring2);
                        spring3.m1 = topology->point(x,y+1,z);
                        spring3.m2 = topology->point(x+1,y,z+1);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring3);
                        spring4.m1 = topology->point(x,y,z+1);
                        spring4.m2 = topology->point(x+1,y+1,z);
                        this->addSpringForce(this->m_potentialEnergy,_f, _x, _v, _f, _x, _v, index++, spring4);
                    }
        }
    }
}

template<class DataTypes>
void RegularGridSpringForceField<DataTypes>::addDForce(const core::MechanicalParams* mparams, DataVecDeriv& df, const DataVecDeriv& dx)
//addDForce(VecDeriv& vdf1, VecDeriv& vdf2, const VecDeriv& vdx1, const VecDeriv& vdx2, double kFactor, double bFactor)
{
    // Calc any custom springs
    this->StiffSpringForceField<DataTypes>::addDForce(mparams, df, dx);
    // Compute topological springs

    VecDeriv& _df = *sofa::helper::getWriteAccessor(df);
    const VecDeriv& _dx = *sofa::helper::getReadAccessor(dx);

    const Real kFactor = static_cast<Real>(sofa::core::mechanicalparams::kFactorIncludingRayleighDamping(
        mparams, this->rayleighStiffness.getValue()));
    const Real bFactor = static_cast<Real>(sofa::core::mechanicalparams::bFactor(mparams));

    const type::vector<Spring>& springs = this->springs.getValue();

    if (topology != nullptr)
    {
        const int nx = topology->getNx();
        const int ny = topology->getNy();
        const int nz = topology->getNz();
        int index = springs.size();
        if (this->linesStiffness.getValue() != 0.0 || this->linesDamping.getValue() != 0.0)
        {
            typename RegularGridSpringForceField<DataTypes>::Spring spring;
            // lines along X
            spring.initpos = (Real)topology->getDx().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x+1,y,z);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring, kFactor, bFactor);
                    }
            // lines along Y
            spring.initpos = (Real)topology->getDy().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x,y+1,z);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring, kFactor, bFactor);
                    }
            // lines along Z
            spring.initpos = (Real)topology->getDz().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x,y,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring, kFactor, bFactor);
                    }

        }
        if (this->quadsStiffness.getValue() != 0.0 || this->quadsDamping.getValue() != 0.0)
        {
            typename RegularGridSpringForceField<DataTypes>::Spring spring1;
            typename RegularGridSpringForceField<DataTypes>::Spring spring2;
            // quads along XY plane
            // lines (x,y,z) -> (x+1,y+1,z)
            spring1.initpos = (Real)(topology->getDx()+topology->getDy()).norm();
            spring1.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring1.kd = this->quadsDamping.getValue() / spring1.initpos;
            // lines (x+1,y,z) -> (x,y+1,z)
            spring2.initpos = (Real)(topology->getDx()-topology->getDy()).norm();
            spring2.ks = this->quadsStiffness.getValue() / spring2.initpos;
            spring2.kd = this->quadsDamping.getValue() / spring2.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x+1,y+1,z);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring1, kFactor, bFactor);
                        spring2.m1 = topology->point(x+1,y,z);
                        spring2.m2 = topology->point(x,y+1,z);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring2, kFactor, bFactor);
                    }
            // quads along XZ plane
            // lines (x,y,z) -> (x+1,y,z+1)
            spring1.initpos = (Real)(topology->getDx()+topology->getDz()).norm();
            spring1.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring1.kd = this->quadsDamping.getValue() / spring1.initpos;
            // lines (x+1,y,z) -> (x,y,z+1)
            spring2.initpos = (Real)(topology->getDx()-topology->getDz()).norm();
            spring2.ks = this->quadsStiffness.getValue() / spring2.initpos;
            spring2.kd = this->quadsDamping.getValue() / spring2.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x+1,y,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring1, kFactor, bFactor);
                        spring2.m1 = topology->point(x+1,y,z);
                        spring2.m2 = topology->point(x,y,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring2, kFactor, bFactor);
                    }
            // quads along YZ plane
            // lines (x,y,z) -> (x,y+1,z+1)
            spring1.initpos = (Real)(topology->getDy()+topology->getDz()).norm();
            spring1.ks = this->quadsStiffness.getValue() / spring1.initpos;
            spring1.kd = this->quadsDamping.getValue() / spring1.initpos;
            // lines (x,y+1,z) -> (x,y,z+1)
            spring1.initpos = (Real)(topology->getDy()-topology->getDz()).norm();
            spring1.ks = this->linesStiffness.getValue() / spring1.initpos;
            spring1.kd = this->linesDamping.getValue() / spring1.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x,y+1,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring1, kFactor, bFactor);
                        spring2.m1 = topology->point(x,y+1,z);
                        spring2.m2 = topology->point(x,y,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring2, kFactor, bFactor);
                    }
        }
        if (this->cubesStiffness.getValue() != 0.0 || this->cubesDamping.getValue() != 0.0)
        {
            typename RegularGridSpringForceField<DataTypes>::Spring spring1;
            typename RegularGridSpringForceField<DataTypes>::Spring spring2;
            typename RegularGridSpringForceField<DataTypes>::Spring spring3;
            typename RegularGridSpringForceField<DataTypes>::Spring spring4;
            // lines (x,y,z) -> (x+1,y+1,z+1)
            spring1.initpos = (Real)(topology->getDx()+topology->getDy()+topology->getDz()).norm();
            spring1.ks = this->cubesStiffness.getValue() / spring1.initpos;
            spring1.kd = this->cubesDamping.getValue() / spring1.initpos;
            // lines (x+1,y,z) -> (x,y+1,z+1)
            spring2.initpos = (Real)(-topology->getDx()+topology->getDy()+topology->getDz()).norm();
            spring2.ks = this->cubesStiffness.getValue() / spring2.initpos;
            spring2.kd = this->cubesDamping.getValue() / spring2.initpos;
            // lines (x,y+1,z) -> (x+1,y,z+1)
            spring3.initpos = (Real)(topology->getDx()-topology->getDy()+topology->getDz()).norm();
            spring3.ks = this->cubesStiffness.getValue() / spring3.initpos;
            spring3.kd = this->cubesDamping.getValue() / spring3.initpos;
            // lines (x,y,z+1) -> (x+1,y+1,z)
            spring4.initpos = (Real)(topology->getDx()+topology->getDy()-topology->getDz()).norm();
            spring4.ks = this->cubesStiffness.getValue() / spring4.initpos;
            spring4.kd = this->cubesDamping.getValue() / spring4.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring1.m1 = topology->point(x,y,z);
                        spring1.m2 = topology->point(x+1,y+1,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring1, kFactor, bFactor);
                        spring2.m1 = topology->point(x+1,y,z);
                        spring2.m2 = topology->point(x,y+1,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring2, kFactor, bFactor);
                        spring3.m1 = topology->point(x,y+1,z);
                        spring3.m2 = topology->point(x+1,y,z+1);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring3, kFactor, bFactor);
                        spring4.m1 = topology->point(x,y,z+1);
                        spring4.m2 = topology->point(x+1,y+1,z);
                        this->addSpringDForce(_df, _dx, _df, _dx, index++, spring4, kFactor, bFactor);
                    }
        }
    }
}



template<class DataTypes>
void RegularGridSpringForceField<DataTypes>::draw(const core::visual::VisualParams* vparams)
{
    using namespace sofa::type;
    using namespace sofa::defaulttype;

    if (!vparams->displayFlags().getShowForceFields()) return;
    assert(this->getMState());

    vparams->drawTool()->saveLastState();

    // Draw any custom springs
    this->StiffSpringForceField<DataTypes>::draw(vparams);
    // Compute topological springs
    const VecCoord& p =this->getMState()->read(core::ConstVecCoordId::position())->getValue();

    std::vector< Vector3 > points;
    Vector3 point1,point2;
    if (topology != nullptr)
    {
        const int nx = topology->getNx();
        const int ny = topology->getNy();
        const int nz = topology->getNz();

        if (this->linesStiffness.getValue() != 0.0 || this->linesDamping.getValue() != 0.0)
        {
            typename RegularGridSpringForceField<DataTypes>::Spring spring;
            // lines along X
            spring.initpos = (Real)topology->getDx().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx-1; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x+1,y,z);
                        point1 = DataTypes::getCPos(p[spring.m1]);
                        point2 = DataTypes::getCPos(p[spring.m2]);
                        points.push_back(point1);
                        points.push_back(point2);
                    }
            // lines along Y
            spring.initpos = (Real)topology->getDy().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz; z++)
                for (int y=0; y<ny-1; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x,y+1,z);
                        point1 = DataTypes::getCPos(p[spring.m1]);
                        point2 = DataTypes::getCPos(p[spring.m2]);
                        points.push_back(point1);
                        points.push_back(point2);
                    }
            // lines along Z
            spring.initpos = (Real)topology->getDz().norm();
            spring.ks = this->linesStiffness.getValue() / spring.initpos;
            spring.kd = this->linesDamping.getValue() / spring.initpos;
            for (int z=0; z<nz-1; z++)
                for (int y=0; y<ny; y++)
                    for (int x=0; x<nx; x++)
                    {
                        spring.m1 = topology->point(x,y,z);
                        spring.m2 = topology->point(x,y,z+1);
                        point1 = DataTypes::getCPos(p[spring.m1]);
                        point2 = DataTypes::getCPos(p[spring.m2]);
                        points.push_back(point1);
                        points.push_back(point2);
                    }

        }
    }

    vparams->drawTool()->drawLines(points, 1, sofa::type::RGBAColor(0.5,0.5,0.5,1));
    vparams->drawTool()->restoreLastState();
}

} // namespace sofa::component::interactionforcefield
