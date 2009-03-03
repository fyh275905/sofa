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
#ifndef SOFA_COMPONENT_FORCEFIELD_MESHSPRINGFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_MESHSPRINGFORCEFIELD_INL

#include <sofa/component/forcefield/MeshSpringForceField.h>
#include <sofa/component/forcefield/StiffSpringForceField.inl>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <iostream>



namespace sofa
{

namespace component
{

namespace forcefield
{

template <class DataTypes>
MeshSpringForceField<DataTypes>::~MeshSpringForceField()
{
}

template <class DataTypes>
double MeshSpringForceField<DataTypes>::getPotentialEnergy()
{
    serr<<"MeshSpringForceField::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}

template<class DataTypes>
void MeshSpringForceField<DataTypes>::addSpring(std::set<std::pair<int,int> >& sset, int m1, int m2, Real stiffness, Real damping)
{
    if (localRange.getValue()[0] >= 0)
    {
        if (m1 < localRange.getValue()[0] || m2 < localRange.getValue()[0]) return;
    }
    if (localRange.getValue()[1] >= 0)
    {
        if (m1 > localRange.getValue()[1] && m2 > localRange.getValue()[1]) return;
    }

    if (m1<m2)
    {
        if (sset.count(std::make_pair(m1,m2))>0) return;
        sset.insert(std::make_pair(m1,m2));
    }
    else
    {
        if (sset.count(std::make_pair(m2,m1))>0) return;
        sset.insert(std::make_pair(m2,m1));
    }
    Real l = ((*this->mstate2->getX0())[m2] - (*this->mstate1->getX0())[m1]).norm();
    this->springs.beginEdit()->push_back(typename SpringForceField<DataTypes>::Spring(m1,m2,stiffness/l, damping/l, l));
    this->springs.endEdit();
}

template<class DataTypes>
void MeshSpringForceField<DataTypes>::init()
{
    this->StiffSpringForceField<DataTypes>::clear();
    if(!(this->mstate1) || !(this->mstate2))
        this->mstate2 = this->mstate1 = dynamic_cast<sofa::core::componentmodel::behavior::MechanicalState<DataTypes> *>(this->getContext()->getMechanicalState());

    if (this->mstate1==this->mstate2)
    {
        sofa::core::componentmodel::topology::BaseMeshTopology* topology = this->mstate1->getContext()->getMeshTopology();

        if (topology != NULL)
        {
            std::set< std::pair<int,int> > sset;
            int n;
            Real s, d;
            if (this->linesStiffness.getValue() != 0.0 || this->linesDamping.getValue() != 0.0)
            {
                s = this->linesStiffness.getValue();
                d = this->linesDamping.getValue();
                n = topology->getNbLines();
                for (int i=0; i<n; ++i)
                {
                    sofa::core::componentmodel::topology::BaseMeshTopology::Line e = topology->getLine(i);
                    this->addSpring(sset, e[0], e[1], s, d);
                }
            }
            if (this->trianglesStiffness.getValue() != 0.0 || this->trianglesDamping.getValue() != 0.0)
            {
                s = this->trianglesStiffness.getValue();
                d = this->trianglesDamping.getValue();
                n = topology->getNbTriangles();
                for (int i=0; i<n; ++i)
                {
                    sofa::core::componentmodel::topology::BaseMeshTopology::Triangle e = topology->getTriangle(i);
                    this->addSpring(sset, e[0], e[1], s, d);
                    this->addSpring(sset, e[0], e[2], s, d);
                    this->addSpring(sset, e[1], e[2], s, d);
                }
            }
            if (this->quadsStiffness.getValue() != 0.0 || this->quadsDamping.getValue() != 0.0)
            {
                s = this->quadsStiffness.getValue();
                d = this->quadsDamping.getValue();
                n = topology->getNbQuads();
                for (int i=0; i<n; ++i)
                {
                    sofa::core::componentmodel::topology::BaseMeshTopology::Quad e = topology->getQuad(i);
                    this->addSpring(sset, e[0], e[1], s, d);
                    this->addSpring(sset, e[0], e[2], s, d);
                    this->addSpring(sset, e[0], e[3], s, d);
                    this->addSpring(sset, e[1], e[2], s, d);
                    this->addSpring(sset, e[1], e[3], s, d);
                    this->addSpring(sset, e[2], e[3], s, d);
                }
            }
            if (this->tetrasStiffness.getValue() != 0.0 || this->tetrasDamping.getValue() != 0.0)
            {
                s = this->tetrasStiffness.getValue();
                d = this->tetrasDamping.getValue();
                n = topology->getNbTetras();
                for (int i=0; i<n; ++i)
                {
                    sofa::core::componentmodel::topology::BaseMeshTopology::Tetra e = topology->getTetra(i);
                    this->addSpring(sset, e[0], e[1], s, d);
                    this->addSpring(sset, e[0], e[2], s, d);
                    this->addSpring(sset, e[0], e[3], s, d);
                    this->addSpring(sset, e[1], e[2], s, d);
                    this->addSpring(sset, e[1], e[3], s, d);
                    this->addSpring(sset, e[2], e[3], s, d);
                }
            }

            if (this->cubesStiffness.getValue() != 0.0 || this->cubesDamping.getValue() != 0.0)
            {
                s = this->cubesStiffness.getValue();
                d = this->cubesDamping.getValue();
#ifdef SOFA_NEW_HEXA
                n = topology->getNbHexas();
                for (int i=0; i<n; ++i)
                {
                    if (!topology->isCubeActive(i)) continue;
                    sofa::core::componentmodel::topology::BaseMeshTopology::Hexa e = topology->getHexa(i);
#else
                n = topology->getNbCubes();
                for (int i=0; i<n; ++i)
                {
                    if (!topology->isCubeActive(i)) continue;
                    sofa::core::componentmodel::topology::BaseMeshTopology::Cube e = topology->getCube(i);
#endif
                    for (int i=0; i<8; i++)
                        for (int j=i+1; j<8; j++)
                        {
                            this->addSpring(sset, e[i], e[j], s, d);
                        }
                }
            }
        }
    }
    this->StiffSpringForceField<DataTypes>::init();
}


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
