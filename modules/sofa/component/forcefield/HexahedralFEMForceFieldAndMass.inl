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
#ifndef SOFA_COMPONENT_FORCEFIELD_HEXAHEDRALFEMFORCEFIELDANDMASS_INL
#define SOFA_COMPONENT_FORCEFIELD_HEXAHEDRALFEMFORCEFIELDANDMASS_INL


#include <sofa/component/forcefield/HexahedralFEMForceFieldAndMass.h>
#include <sofa/component/forcefield/HexahedralFEMForceField.inl>

#include <sofa/component/topology/PointData.inl>
#include <sofa/component/topology/HexahedronData.inl>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;

template<class DataTypes>
HexahedralFEMForceFieldAndMass<DataTypes>::HexahedralFEMForceFieldAndMass()
    : MassT()
    , HexahedralFEMForceFieldT()
    , _density(initData(&_density,(Real)1.0,"density","density == volumetric mass in english (kg.m-3)"))
//	, _elementMasses(initData(&_elementMasses,"massMatrices", "Mass matrices per element (M_i)"))
//	, _particleMasses(initData(&_particleMasses, "particleMasses", "Mass per particle"))
//	, _lumpedMasses(initData(&_lumpedMasses, "lumpedMasses", "Lumped masses"))
{
    _useLumpedMass = this->initData(&_useLumpedMass, false, "lumpedMass", "Does it use lumped masses?");
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::init( )
{
    this->core::componentmodel::behavior::ForceField<DataTypes>::init();

    this->getContext()->get(this->_topology);

    if(this->_topology == NULL)
    {
        serr << "ERROR(HexahedralFEMForceField): object must have a HexahedronSetTopology."<<sendl;
        return;
    }

    this->reinit();
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::reinit( )
{
    HexahedralFEMForceFieldT::reinit();
    MassT::reinit();

    computeElementMasses();
    computeParticleMasses();
    computeLumpedMasses();
}


template <class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::handleTopologyChange(core::componentmodel::topology::Topology* t)
{
    if(t != this->_topology)
        return;

    HexahedralFEMForceFieldT::handleTopologyChange();

    std::list<const TopologyChange *>::const_iterator itBegin=this->_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=this->_topology->lastChange();

    // handle point events
    _particleMasses.handleTopologyEvents(itBegin,itEnd);

    if( _useLumpedMass.getValue() )
        _lumpedMasses.handleTopologyEvents(itBegin,itEnd);

    // handle hexa events
    _elementMasses.handleTopologyEvents(itBegin,itEnd);
    _elementTotalMass.handleTopologyEvents(itBegin,itEnd);

    for(std::list<const TopologyChange *>::const_iterator iter = itBegin;
        iter != itEnd; ++iter)
    {
        switch((*iter)->getChangeType())
        {
            // for added elements:
            // compute ElementMasses and TotalMass
            // add particle masses and lumped masses of adjacent particles
        case HEXAHEDRAADDED:
        {
            const VecElement& hexas = this->_topology->getHexas();
            const sofa::helper::vector<unsigned int> &hexaModif = (static_cast< const HexahedraAdded *> (*iter))->hexahedronIndexArray;

            VecCoord& initialPoints = *this->mstate->getX0();

            helper::vector<ElementMass>& elementMasses = *this->_elementMasses.beginEdit();
            helper::vector<Real>& elementTotalMass = *this->_elementTotalMass.beginEdit();

            for(unsigned int i=0; i<hexaModif.size(); ++i)
            {
                const unsigned int hexaId = hexaModif[i];

                Vec<8,Coord> nodes;
                for(int w=0; w<8; ++w)
                    nodes[w] = initialPoints[hexas[hexaId][w]];

                computeElementMass( elementMasses[hexaId], elementTotalMass[hexaId],
                        this->hexahedronInfo.getValue()[hexaId].rotatedInitialElements);
            }

            this->_elementTotalMass.endEdit();
            this->_elementMasses.endEdit();


            helper::vector<Real>&	particleMasses = *this->_particleMasses.beginEdit();

            for(unsigned int i=0; i<hexaModif.size(); ++i)
            {
                const unsigned int hexaId = hexaModif[i];

                Real mass = _elementTotalMass.getValue()[hexaId] * (Real) 0.125;

                for(int w=0; w<8; ++w)
                    particleMasses[ hexas[hexaId][w] ] += mass;
            }

            this->_particleMasses.endEdit();

            if( _useLumpedMass.getValue() )
            {
                helper::vector<Coord>&	lumpedMasses = *this->_lumpedMasses.beginEdit();

                for(unsigned int i=0; i<hexaModif.size(); ++i)
                {
                    const unsigned int hexaId = hexaModif[i];
                    const ElementMass& mass = this->_elementMasses.getValue()[hexaId];

                    for(int w=0; w<8; ++w)
                    {
                        for(int j=0; j<8*3; ++j)
                        {
                            lumpedMasses[ hexas[hexaId][w] ][0] += mass[w*3  ][j];
                            lumpedMasses[ hexas[hexaId][w] ][1] += mass[w*3+1][j];
                            lumpedMasses[ hexas[hexaId][w] ][2] += mass[w*3+2][j];
                        }
                    }
                }

                this->_lumpedMasses.endEdit();
            }

        }
        break;

        // for removed elements:
        // subttract particle masses and lumped masses of adjacent particles
        case HEXAHEDRAREMOVED:
        {
            const VecElement& hexas = this->_topology->getHexas();
            const sofa::helper::vector<unsigned int> &hexaModif = (static_cast< const HexahedraRemoved *> (*iter))->getArray();

            helper::vector<Real>&	particleMasses = *this->_particleMasses.beginEdit();

            for(unsigned int i=0; i<hexaModif.size(); ++i)
            {
                const unsigned int hexaId = hexaModif[i];

                Real mass = _elementTotalMass.getValue()[hexaId] * (Real) 0.125;

                for(int w=0; w<8; ++w)
                    particleMasses[ hexas[hexaId][w] ] -= mass;
            }

            this->_particleMasses.endEdit();

            if( _useLumpedMass.getValue() )
            {
                helper::vector<Coord>&	lumpedMasses = *this->_lumpedMasses.beginEdit();

                for(unsigned int i=0; i<hexaModif.size(); ++i)
                {
                    const unsigned int hexaId = hexaModif[i];
                    const ElementMass& mass = this->_elementMasses.getValue()[hexaId];

                    for(int w=0; w<8; ++w)
                    {
                        for(int j=0; j<8*3; ++j)
                        {
                            lumpedMasses[ hexas[hexaId][w] ][0] -= mass[w*3  ][j];
                            lumpedMasses[ hexas[hexaId][w] ][1] -= mass[w*3+1][j];
                            lumpedMasses[ hexas[hexaId][w] ][2] -= mass[w*3+2][j];
                        }
                    }
                }

                this->_lumpedMasses.endEdit();
            }
        }
        break;
        default:
            break;
        }
    }
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::computeParticleMasses(  )
{
    unsigned int numPoints = this->_topology->getNbPoints();
    const VecElement& hexas = this->_topology->getHexas();

    helper::vector<Real>&	particleMasses = *this->_particleMasses.beginEdit();

    particleMasses.clear();
    particleMasses.resize( numPoints );

    for(unsigned int i=0; i<hexas.size(); ++i)
    {
        // mass of a particle...
        Real mass = _elementTotalMass.getValue()[i] * (Real) 0.125;

        // ... is added to each particle of the element
        for(int w=0; w<8; ++w)
            particleMasses[ hexas[i][w] ] += mass;
    }

    this->_particleMasses.endEdit();
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::computeLumpedMasses(  )
{
    unsigned int numPoints = this->_topology->getNbPoints();
    const VecElement& hexas = this->_topology->getHexas();

    if( _useLumpedMass.getValue() )
    {
        helper::vector<Coord>&	lumpedMasses = *this->_lumpedMasses.beginEdit();

        lumpedMasses.clear();
        lumpedMasses.resize( numPoints, Coord(0.0, 0.0, 0.0) );

        for(unsigned int i=0; i<hexas.size(); ++i)
        {
            const ElementMass& mass = this->_elementMasses.getValue()[i];

            for(int w=0; w<8; ++w)
            {
                for(int j=0; j<8*3; ++j)
                {
                    lumpedMasses[ hexas[i][w] ][0] += mass[w*3  ][j];
                    lumpedMasses[ hexas[i][w] ][1] += mass[w*3+1][j];
                    lumpedMasses[ hexas[i][w] ][2] += mass[w*3+2][j];
                }
            }
        }

        this->_lumpedMasses.endEdit();
    }
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::computeElementMasses(  )
{
    VecCoord& initialPoints = *this->mstate->getX0();

    const VecElement& hexas = this->_topology->getHexas();

    helper::vector<ElementMass>& elementMasses = *this->_elementMasses.beginEdit();
    helper::vector<Real>& elementTotalMass = *this->_elementTotalMass.beginEdit();

    elementMasses.resize( hexas.size() );
    elementTotalMass.resize( hexas.size() );

    for(unsigned int i=0; i<hexas.size(); ++i)
    {
        Vec<8,Coord> nodes;
        for(int w=0; w<8; ++w)
            nodes[w] = initialPoints[hexas[i][w]];

        computeElementMass( elementMasses[i], elementTotalMass[i],
                this->hexahedronInfo.getValue()[i].rotatedInitialElements);
    }

    this->_elementTotalMass.endEdit();
    this->_elementMasses.endEdit();
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::computeElementMass( ElementMass &Mass, Real& totalMass,
        const helper::fixed_array<Coord,8> &nodes)
{
    // volume of a element
    Real volume = (nodes[1]-nodes[0]).norm()*(nodes[3]-nodes[0]).norm()*(nodes[4]-nodes[0]).norm();

    // total element mass
    totalMass = volume * _density.getValue();

    Coord l = nodes[6] - nodes[0];

    Mass.clear();

    for(int i=0; i<8; ++i)
    {
        Real mass = totalMass * integrateVolume(this->_coef[i][0],
                this->_coef[i][1],
                this->_coef[i][2],
                2.0f/l[0],
                2.0f/l[1],
                2.0f/l[2]);

        Mass[i*3][i*3] += mass;
        Mass[i*3+1][i*3+1] += mass;
        Mass[i*3+2][i*3+2] += mass;

        for(int j=i+1; j<8; ++j)
        {
            Real mass = totalMass * integrateVolume(this->_coef[i][0],
                    this->_coef[i][1],
                    this->_coef[i][2],
                    2.0f/l[0],
                    2.0f/l[1],
                    2.0f/l[2]);

            Mass[i*3][j*3] += mass;
            Mass[i*3+1][j*3+1] += mass;
            Mass[i*3+2][j*3+2] += mass;
        }
    }

    for(int i=0; i<24; ++i)
        for(int j=i+1; j<24; ++j)
        {
            Mass[j][i] = Mass[i][j];
        }
}


template<class DataTypes>
typename HexahedralFEMForceFieldAndMass<DataTypes>::Real HexahedralFEMForceFieldAndMass<DataTypes>::integrateVolume(  int signx, int signy, int signz,Real /*l0*/,Real /*l1*/,Real /*l2*/  )
{
    Real t1 = (Real)(signx*signx);
    Real t2 = (Real)(signy*signy);
    Real t3 = (Real)(signz*signz);
    Real t9 = (Real)(t1*t2);

    return (Real)(t1*t3/72.0+t2*t3/72.0+t9*t3/216.0+t3/24.0+1.0/8.0+t9/72.0+t1/24.0+t2/24.0);
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::addMDx(VecDeriv& f, const VecDeriv& dx, double factor)
{
    if( ! _useLumpedMass.getValue() )
    {
        const VecElement& hexas = this->_topology->getHexas();
        for(unsigned int i=0; i<hexas.size(); ++i)
        {
            Vec<24, Real> actualDx, actualF;

            for(int k=0 ; k<8 ; ++k )
            {
                int indice = k*3;
                for(int j=0 ; j<3 ; ++j )
                    actualDx[indice+j] = dx[hexas[i][k]][j];
            }

            actualF = _elementMasses.getValue()[i] * actualDx;


            for(unsigned int w=0; w<8; ++w)
                f[hexas[i][w]] += Deriv( actualF[w*3],  actualF[w*3+1],   actualF[w*3+2]  ) * factor;
        }
    }
    else // lumped matrices
    {
        for(unsigned int i=0; i<_lumpedMasses.getValue().size(); ++i)
            for(unsigned int j=0; j<3; ++j)
                f[i][j] += (Real)(_lumpedMasses.getValue()[i][j] * dx[i][j] *factor);
    }
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::addMToMatrix(defaulttype::BaseMatrix * mat, double mFact, unsigned int &offset)
{
    // Build Matrix Block for this ForceField
    int i, j, n1, n2;
    int node1, node2;

    const VecElement& hexas = this->_topology->getHexas();

    for(unsigned int e=0; e<hexas.size(); ++e)
    {
        const ElementMass &Me = _elementMasses.getValue()[e];

        // find index of node 1
        for (n1=0; n1<8; n1++)
        {
            node1 = hexas[e][n1];

            // find index of node 2
            for (n2=0; n2<8; n2++)
            {
                node2 = hexas[e][n2];

                Mat33 tmp = Mat33(Coord(Me[3*n1+0][3*n2+0],Me[3*n1+0][3*n2+1],Me[3*n1+0][3*n2+2]),
                        Coord(Me[3*n1+1][3*n2+0],Me[3*n1+1][3*n2+1],Me[3*n1+1][3*n2+2]),
                        Coord(Me[3*n1+2][3*n2+0],Me[3*n1+2][3*n2+1],Me[3*n1+2][3*n2+2]));
                for(i=0; i<3; i++)
                    for (j=0; j<3; j++)
                        mat->add(offset+3*node1+i, offset+3*node2+j, tmp[i][j]*mFact);
            }
        }
    }
}

template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::accFromF(VecDeriv& /*a*/, const VecDeriv& /*f*/)
{
    serr<<"HexahedralFEMForceFieldAndMass<DataTypes>::accFromF not yet implemented"<<sendl;
    // need to built the big global mass matrix and to inverse it...
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::addGravityToV(double dt)
{
    if(this->mstate)
    {
        VecDeriv& v = *this->mstate->getV();
        for (unsigned int i=0; i<_particleMasses.getValue().size(); i++)
        {
            v[i] +=this->getContext()->getLocalGravity()*dt;
        }
    }
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v)
{
    HexahedralFEMForceFieldT::addForce(f,x,v);

    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if (this->m_separateGravity.getValue())
        return;

    for (unsigned int i=0; i<_particleMasses.getValue().size(); i++)
    {
        f[i] += this->getContext()->getLocalGravity()*_particleMasses.getValue()[i];
    }
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx)
{
    HexahedralFEMForceFieldT::addDForce(df,dx);
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx, double kFactor, double)
{
    VecDeriv kdx(dx.size());// = dx * kFactor;
    for(unsigned i=0; i<dx.size(); ++i)
        kdx[i]=dx[i]*kFactor;
    HexahedralFEMForceFieldT::addDForce(df,kdx);
}


template<class DataTypes>
double HexahedralFEMForceFieldAndMass<DataTypes>::getElementMass(unsigned int /*index*/)
{
    serr<<"HexahedralFEMForceFieldAndMass<DataTypes>::getElementMass not yet implemented"<<sendl; return 0.0;
}


template<class DataTypes>
void HexahedralFEMForceFieldAndMass<DataTypes>::draw()
{
    HexahedralFEMForceFieldT::draw();

    if (!this->getContext()->getShowBehaviorModels())
        return;
    const VecCoord& x = *this->mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(2);
    glColor4f (1,1,1,1);
    glBegin (GL_POINTS);
    for (unsigned int i=0; i<x.size(); i++)
    {
        helper::gl::glVertexT(x[i]);
    }
    glEnd();
}


template<class DataTypes>
bool HexahedralFEMForceFieldAndMass<DataTypes>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    for (unsigned int i=0; i<x.size(); i++)
    {
        Real p[3] = {0.0, 0.0, 0.0};
        DataTypes::get(p[0],p[1],p[2],x[i]);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

}
}
}


#endif
