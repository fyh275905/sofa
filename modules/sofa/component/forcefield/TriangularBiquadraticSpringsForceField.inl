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
#include <sofa/component/forcefield/TriangularBiquadraticSpringsForceField.h>
#include <fstream> // for reading the file
#include <iostream> //for debugging
#include <sofa/helper/gl/template.h>
#include <sofa/component/topology/TriangleData.inl>
#include <sofa/component/topology/EdgeData.inl>
#include <sofa/component/topology/TriangleSetGeometryAlgorithms.h>
#include <sofa/core/componentmodel/behavior/ForceField.inl>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using namespace	sofa::component::topology;
using namespace core::componentmodel::topology;





using core::componentmodel::topology::BaseMeshTopology;

typedef BaseMeshTopology::Triangle			Triangle;
typedef BaseMeshTopology::TriangleEdges		TriangleEdges;


template< class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::TRBSEdgeCreationFunction(int edgeIndex, void* param, EdgeRestInformation &ei,
        const Edge& ,  const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    TriangularBiquadraticSpringsForceField<DataTypes> *ff= (TriangularBiquadraticSpringsForceField<DataTypes> *)param;
    if (ff)
    {

        sofa::component::topology::TriangleSetGeometryAlgorithms<DataTypes>* triangleGeo;
        ff->getContext()->get(triangleGeo);

        // store the rest length of the edge created
        ei.restSquareLength=triangleGeo->computeRestSquareEdgeLength(edgeIndex);
        ei.stiffness=0;
    }
}



template< class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::TRBSTriangleCreationFunction (int triangleIndex, void* param,
        TriangleRestInformation &tinfo,
        const Triangle& ,
        const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    TriangularBiquadraticSpringsForceField<DataTypes> *ff= (TriangularBiquadraticSpringsForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int j,k,l;

        EdgeData<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation> &edgeInfo=ff->getEdgeInfo();
        typename DataTypes::Real area,restSquareLength[3],cotangent[3];
        typename DataTypes::Real lambda=ff->getLambda();
        typename DataTypes::Real mu=ff->getMu();

        helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

        /// describe the jth edge index of triangle no i
        const TriangleEdges &te= ff->_topology->getEdgeTriangleShell(triangleIndex);
        // store square rest length
        for(j=0; j<3; ++j)
        {
            restSquareLength[j]=edgeInf[te[j]].restSquareLength;
        }
        // compute rest area based on Heron's formula
        area=0;
        for(j=0; j<3; ++j)
        {
            area+=restSquareLength[j]*(restSquareLength[(j+1)%3] +restSquareLength[(j+2)%3]-restSquareLength[j]);
        }
        area=sqrt(area)/4;

        for(j=0; j<3; ++j)
        {
            cotangent[j]=(restSquareLength[(j+1)%3] +restSquareLength[(j+2)%3]-restSquareLength[j])/(4*area);
            if (ff->f_printLog.getValue())
            {
                if (cotangent[j]<0)
                    std::cerr<<"negative cotangent["<<triangleIndex<<"]["<<j<<"]"<<std::endl;
            }
        }
        for(j=0; j<3; ++j)
        {
            k=(j+1)%3;
            l=(j+2)%3;
            tinfo.gamma[j]=(2*cotangent[k]*cotangent[l]*(lambda+mu)-mu)/(16*area);
            tinfo.stiffness[j]=(2*cotangent[j]*cotangent[j]*(lambda+mu)+mu)/(16*area);
            edgeInf[te[j]].stiffness+=tinfo.stiffness[j];
        }
        edgeInfo.endEdit();
    }

}


template< class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::TRBSTriangleDestroyFunction(int triangleIndex, void* param, typename TriangularBiquadraticSpringsForceField<DataTypes>::TriangleRestInformation &tinfo)
{
    TriangularBiquadraticSpringsForceField<DataTypes> *ff= (TriangularBiquadraticSpringsForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int j;

        EdgeData<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation> &edgeInfo=ff->getEdgeInfo();

        helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

        /// describe the jth edge index of triangle no i
        const TriangleEdges &te= ff->_topology->getEdgeTriangleShell(triangleIndex);
        // store square rest length
        for(j=0; j<3; ++j)
        {
            edgeInf[te[j]].stiffness -= tinfo.stiffness[j];
        }

    }
}
template <class DataTypes> TriangularBiquadraticSpringsForceField<DataTypes>::TriangularBiquadraticSpringsForceField()
    : _initialPoints(initData(&_initialPoints,"initialPoints", "Initial Position"))
    , updateMatrix(true)
    , f_poissonRatio(initData(&f_poissonRatio,(Real)0.3,"poissonRatio","Poisson ratio in Hooke's law"))
    , f_youngModulus(initData(&f_youngModulus,(Real)1000.,"youngModulus","Young modulus in Hooke's law"))
    , f_dampingRatio(initData(&f_dampingRatio,(Real)0.,"dampingRatio","Ratio damping/stiffness"))
    , f_useAngularSprings(initData(&f_useAngularSprings,true,"useAngularSprings","If Angular Springs should be used or not"))
    , lambda(0)
    , mu(0)
{
}

template <class DataTypes> void TriangularBiquadraticSpringsForceField<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    edgeInfo.handleTopologyEvents(itBegin,itEnd);
    triangleInfo.handleTopologyEvents(itBegin,itEnd);
}

template <class DataTypes> TriangularBiquadraticSpringsForceField<DataTypes>::~TriangularBiquadraticSpringsForceField()
{

}

template <class DataTypes> void TriangularBiquadraticSpringsForceField<DataTypes>::init()
{
    serr << "initializing TriangularBiquadraticSpringsForceField" << sendl;
    this->Inherited::init();

    _topology = getContext()->getMeshTopology();

    if (_topology->getNbTriangles()==0)
    {
        serr << "ERROR(TriangularBiquadraticSpringsForceField): object must have a Triangular Set Topology."<<sendl;
        return;
    }
    updateLameCoefficients();

    /// prepare to store info in the triangle array
    helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::TriangleRestInformation>& triangleInf = *(triangleInfo.beginEdit());

    triangleInf.resize(_topology->getNbTriangles());
    /// prepare to store info in the edge array
    helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

    edgeInf.resize(_topology->getNbEdges());

    // get restPosition
    if (_initialPoints.getValue().size() == 0)
    {
        VecCoord& p = *this->mstate->getX0();
        _initialPoints.setValue(p);
    }
    int i;
    for (i=0; i<_topology->getNbEdges(); ++i)
    {
        TRBSEdgeCreationFunction(i, (void*) this, edgeInf[i],
                _topology->getEdge(i),  (const sofa::helper::vector< unsigned int > )0,
                (const sofa::helper::vector< double >)0);
    }
    for (i=0; i<_topology->getNbTriangles(); ++i)
    {
        TRBSTriangleCreationFunction(i, (void*) this, triangleInf[i],
                _topology->getTriangle(i),  (const sofa::helper::vector< unsigned int > )0,
                (const sofa::helper::vector< double >)0);
    }

    edgeInfo.setCreateFunction(TRBSEdgeCreationFunction);
    triangleInfo.setCreateFunction(TRBSTriangleCreationFunction);
    triangleInfo.setDestroyFunction(TRBSTriangleDestroyFunction);
    edgeInfo.setCreateParameter( (void *) this );
    edgeInfo.setDestroyParameter( (void *) this );
    triangleInfo.setCreateParameter( (void *) this );
    triangleInfo.setDestroyParameter( (void *) this );

    edgeInfo.endEdit();
    triangleInfo.endEdit();
}


template <class DataTypes>
double TriangularBiquadraticSpringsForceField<DataTypes>::getPotentialEnergy(const VecCoord& /*x*/)
{
    serr<<"TriangularBiquadraticSpringsForceField::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}
template <class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v)
{
    unsigned int j,k,l,v0,v1;
    int nbEdges=_topology->getNbEdges();
    int nbTriangles=_topology->getNbTriangles();

    Real val,L;
    TriangleRestInformation *tinfo;
    EdgeRestInformation *einfo;

    helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::TriangleRestInformation>& triangleInf = *(triangleInfo.beginEdit());

    helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

    assert(this->mstate);

    Deriv force;
    Coord dp,dv;
    Real _dampingRatio=f_dampingRatio.getValue();


    for(int i=0; i<nbEdges; i++ )
    {
        einfo=&edgeInf[i];
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];
        dp=x[v0]-x[v1];
        dv=v[v0]-v[v1];
        L=einfo->currentSquareLength=dp.norm2();
        einfo->deltaL2=einfo->currentSquareLength-einfo->restSquareLength +_dampingRatio*dot(dv,dp)/L;

        val=einfo->stiffness*einfo->deltaL2;
        force=dp*val;
        f[v1]+=force;
        f[v0]-=force;
    }
    if (f_useAngularSprings.getValue()==true)
    {
        for(int i=0; i<nbTriangles; i++ )
        {
            tinfo=&triangleInf[i];
            /// describe the jth edge index of triangle no i
            const TriangleEdges &tea= _topology->getEdgeTriangleShell(i);
            /// describe the jth vertex index of triangle no i
            const Triangle &ta= _topology->getTriangle(i);

            // store points
            for(j=0; j<3; ++j)
            {
                k=(j+1)%3;
                l=(j+2)%3;
                force=(x[ta[k]] - x[ta[l]])*
                        (edgeInf[tea[k]].deltaL2 * tinfo->gamma[l] +edgeInf[tea[l]].deltaL2 * tinfo->gamma[k]);
                f[ta[l]]+=force;
                f[ta[k]]-=force;
            }
        }
        //	serr << "tinfo->gamma[0] "<<tinfo->gamma[0]<<sendl;

    }
    edgeInfo.endEdit();
    triangleInfo.endEdit();
    updateMatrix=true;
}


template <class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx)
{
    unsigned int i,j,k;
    int nbTriangles=_topology->getNbTriangles();

    TriangleRestInformation *tinfo;

//	serr << "start addDForce" << sendl;


    assert(this->mstate);
    VecDeriv& x = *this->mstate->getX();


    Deriv deltax,res;

    helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::TriangleRestInformation>& triangleInf = *(triangleInfo.beginEdit());

    helper::vector<typename TriangularBiquadraticSpringsForceField<DataTypes>::EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());


    if (updateMatrix)
    {
        int u,v;
        Real val1,val2,vali,valj,valk;
        Coord dpj,dpk,dpi;

        //	serr <<"updating matrix"<<sendl;
        updateMatrix=false;
        for(int l=0; l<nbTriangles; l++ )
        {
            tinfo=&triangleInf[l];
            /// describe the jth edge index of triangle no i
            const TriangleEdges &tea= _topology->getEdgeTriangleShell(l);
            /// describe the jth vertex index of triangle no i
            const Triangle &ta= _topology->getTriangle(l);

            // store points
            for(k=0; k<3; ++k)
            {
                i=(k+1)%3;
                j=(k+2)%3;
                Mat3 &m=tinfo->DfDx[k];
                dpk = x[ta[i]]- x[ta[j]];

                if (f_useAngularSprings.getValue()==false)
                {
                    val1 = -tinfo->stiffness[k]*edgeInf[tea[k]].deltaL2;

                    val2= -2*tinfo->stiffness[k];

                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[u][v]=dpk[u]*dpk[v]*val2;
                        }
                        m[u][u]+=val1;
                    }

                }
                else
                {
                    dpj = x[ta[i]]- x[ta[k]];
                    dpi = x[ta[j]]- x[ta[k]];

                    val1 = -(tinfo->stiffness[k]*edgeInf[tea[k]].deltaL2+
                            tinfo->gamma[i]*edgeInf[tea[j]].deltaL2+
                            tinfo->gamma[j]*edgeInf[tea[i]].deltaL2);

                    val2= -2*tinfo->stiffness[k];
                    valk=2*tinfo->gamma[k];
                    vali=2*tinfo->gamma[i];
                    valj=2*tinfo->gamma[j];


                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[u][v]=dpk[u]*dpk[v]*val2
                                    +dpj[u]*dpi[v]*valk
                                    -dpj[u]*dpk[v]*vali
                                    +dpk[u]*dpi[v]*valj;

                        }
                        m[u][u]+=val1;
                    }
                }
            }
        }

    }

    for(int l=0; l<nbTriangles; l++ )
    {
        tinfo=&triangleInf[l];
        /// describe the jth vertex index of triangle no l
        const Triangle &ta= _topology->getTriangle(l);

        // store points
        for(k=0; k<3; ++k)
        {
            i=(k+1)%3;
            j=(k+2)%3;
            deltax= dx[ta[i]] -dx[ta[j]];
            res=tinfo->DfDx[k]*deltax;
            df[ta[i]]+=res;
            df[ta[j]]-= tinfo->DfDx[k].transposeMultiply(deltax);
        }
    }
    edgeInfo.endEdit();
    triangleInfo.endEdit();
}


template<class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::updateLameCoefficients()
{
    lambda= f_youngModulus.getValue()*f_poissonRatio.getValue()/(1-f_poissonRatio.getValue()*f_poissonRatio.getValue());
    mu = f_youngModulus.getValue()*(1-f_poissonRatio.getValue())/(1-f_poissonRatio.getValue()*f_poissonRatio.getValue());
//	serr << "initialized Lame coef : lambda=" <<lambda<< " mu="<<mu<<sendl;
}


template<class DataTypes>
void TriangularBiquadraticSpringsForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!this->mstate) return;

    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    VecCoord& x = *this->mstate->getX();
    int nbTriangles=_topology->getNbTriangles();

    glDisable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);
    for(int i=0; i<nbTriangles; ++i)
    {
        int a = _topology->getTriangle(i)[0];
        int b = _topology->getTriangle(i)[1];
        int c = _topology->getTriangle(i)[2];

        glColor4f(0,1,0,1);
        helper::gl::glVertexT(x[a]);
        glColor4f(0,0.5,0.5,1);
        helper::gl::glVertexT(x[b]);
        glColor4f(0,0,1,1);
        helper::gl::glVertexT(x[c]);
    }
    glEnd();


    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

} // namespace forcefield

} // namespace Components

} // namespace Sofa
