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
#include <sofa/component/forcefield/TriangularTensorMassForceField.h>
#include <fstream> // for reading the file
#include <iostream> //for debugging
#include <sofa/helper/gl/template.h>
#include <sofa/component/topology/TriangleData.inl>
#include <sofa/component/topology/EdgeData.inl>
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
typedef BaseMeshTopology::TriangleEdges TriangleEdges;

template< class DataTypes>
void TriangularTensorMassForceField<DataTypes>::TriangularTMEdgeCreationFunction(int /*edgeIndex*/, void* param, EdgeRestInformation &ei,
        const Edge& ,  const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    TriangularTensorMassForceField<DataTypes> *ff= (TriangularTensorMassForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int u,v;
        /// set to zero the stiffness matrix
        for (u=0; u<3; ++u)
        {
            for (v=0; v<3; ++v)
            {
                ei.DfDx[u][v]=0;
            }
        }

    }
}

template< class DataTypes>
void TriangularTensorMassForceField<DataTypes>::TriangularTMTriangleCreationFunction (const sofa::helper::vector<unsigned int> &triangleAdded,
        void* param, vector<EdgeRestInformation> &edgeData)
{
    TriangularTensorMassForceField<DataTypes> *ff= (TriangularTensorMassForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int i,j,k,l,u,v;

        typename DataTypes::Real val1,area,restSquareLength[3],cotangent[3];
        typename DataTypes::Real lambda=ff->getLambda();
        typename DataTypes::Real mu=ff->getMu();
        typename DataTypes::Real lambdastar, mustar;
        typename DataTypes::Coord point[3],dpk,dpl;

        const typename DataTypes::VecCoord *restPosition=ff->mstate->getX0();

        for (i=0; i<triangleAdded.size(); ++i)
        {

            /// describe the jth edge index of triangle no i
            const TriangleEdges &te= ff->_topology->getEdgeTriangleShell(triangleAdded[i]);
            /// describe the jth vertex index of triangle no i
            const Triangle &t= ff->_topology->getTriangle(triangleAdded[i]);
            // store points
            for(j=0; j<3; ++j)
                point[j]=(*restPosition)[t[j]];
            // store square rest length
            for(j=0; j<3; ++j)
            {
                restSquareLength[j]= (point[(j+1)%3] -point[(j+2)%3]).norm2();
            }
            // compute rest area based on Heron's formula
            area=0;
            for(j=0; j<3; ++j)
            {
                area+=restSquareLength[j]*(restSquareLength[(j+1)%3] +restSquareLength[(j+2)%3]-restSquareLength[j]);
            }
            area=sqrt(area)/4;
            lambdastar=lambda/(4*area);
            mustar=mu/(8*area);

            for(j=0; j<3; ++j)
            {
                cotangent[j]=(restSquareLength[(j+1)%3] +restSquareLength[(j+2)%3]-restSquareLength[j])/(4*area);
                if (ff->f_printLog.getValue())
                {
                    if (cotangent[j]<0)
                        std::cerr<<"negative cotangent["<<triangleAdded[i]<<"]["<<j<<"]"<<std::endl;
                }
            }
            for(j=0; j<3; ++j)
            {
                k=(j+1)%3;
                l=(j+2)%3;
                Mat3 &m=edgeData[te[j]].DfDx;
                dpl= point[j]-point[k];
                dpk= point[j]-point[l];
                val1= -cotangent[j]*(lambda+mu)/2;

                if (ff->_topology->getEdge(te[j])[0]==t[l])
                {
                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[u][v]+= lambdastar*dpl[u]*dpk[v]+mustar*dpk[u]*dpl[v];
                        }
                        m[u][u]+=val1;
                    }
                }
                else
                {
                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[v][u]+= lambdastar*dpl[u]*dpk[v]+mustar*dpk[u]*dpl[v];
                        }
                        m[u][u]+=val1;
                    }
                }
            }

        }

    }
}

template< class DataTypes>
void TriangularTensorMassForceField<DataTypes>::TriangularTMTriangleDestructionFunction (const sofa::helper::vector<unsigned int> &triangleRemoved,
        void* param, vector<EdgeRestInformation> &edgeData)
{
    TriangularTensorMassForceField<DataTypes> *ff= (TriangularTensorMassForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int i,j,k,l,u,v;

        typename DataTypes::Real val1,area,restSquareLength[3],cotangent[3];
        typename DataTypes::Real lambda=ff->getLambda();
        typename DataTypes::Real mu=ff->getMu();
        typename DataTypes::Real lambdastar, mustar;
        typename DataTypes::Coord point[3],dpk,dpl;

        const typename DataTypes::VecCoord *restPosition=ff->mstate->getX0();

        for (i=0; i<triangleRemoved.size(); ++i)
        {

            /// describe the jth edge index of triangle no i
            const TriangleEdges &te= ff->_topology->getEdgeTriangleShell(triangleRemoved[i]);
            /// describe the jth vertex index of triangle no i
            const Triangle &t= ff->_topology->getTriangle(triangleRemoved[i]);
            // store points
            for(j=0; j<3; ++j)
                point[j]=(*restPosition)[t[j]];
            // store square rest length
            for(j=0; j<3; ++j)
            {
                restSquareLength[j]= (point[(j+1)%3] -point[(j+2)%3]).norm2();
            }
            // compute rest area based on Heron's formula
            area=0;
            for(j=0; j<3; ++j)
            {
                area+=restSquareLength[j]*(restSquareLength[(j+1)%3] +restSquareLength[(j+2)%3]-restSquareLength[j]);
            }
            area=sqrt(area)/4;
            lambdastar=lambda/(4*area);
            mustar=mu/(8*area);

            for(j=0; j<3; ++j)
            {
                cotangent[j]=(restSquareLength[(j+1)%3] +restSquareLength[(j+2)%3]-restSquareLength[j])/(4*area);
                if (ff->f_printLog.getValue())
                {
                    if (cotangent[j]<0)
                        std::cerr<<"negative cotangent["<<triangleRemoved[i]<<"]["<<j<<"]"<<std::endl;
                }
            }
            for(j=0; j<3; ++j)
            {
                k=(j+1)%3;
                l=(j+2)%3;
                Mat3 &m=edgeData[te[j]].DfDx;
                dpl= point[j]-point[k];
                dpk= point[j]-point[l];
                val1= -cotangent[j]*(lambda+mu)/2;

                if (ff->_topology->getEdge(te[j])[0]==t[l])
                {
                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[u][v]-= lambdastar*dpl[u]*dpk[v]+mustar*dpk[u]*dpl[v];
                        }
                        m[u][u]-=val1;
                    }
                }
                else
                {
                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[v][u]-= lambdastar*dpl[u]*dpk[v]+mustar*dpk[u]*dpl[v];
                        }
                        m[u][u]-=val1;
                    }
                }
            }

        }

    }
}


template <class DataTypes> TriangularTensorMassForceField<DataTypes>::TriangularTensorMassForceField()
    : _initialPoints(0)
    , updateMatrix(true)
    , f_poissonRatio(initData(&f_poissonRatio,(Real)0.3,"poissonRatio","Poisson ratio in Hooke's law"))
    , f_youngModulus(initData(&f_youngModulus,(Real)1000.,"youngModulus","Young modulus in Hooke's law"))
    , lambda(0)
    , mu(0)
{
}

template <class DataTypes> void TriangularTensorMassForceField<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    edgeInfo.handleTopologyEvents(itBegin,itEnd);
}

template <class DataTypes> TriangularTensorMassForceField<DataTypes>::~TriangularTensorMassForceField()
{

}

template <class DataTypes> void TriangularTensorMassForceField<DataTypes>::init()
{
    serr << "initializing TriangularTensorMassForceField" << sendl;
    this->Inherited::init();

    _topology = getContext()->getMeshTopology();

    if (_topology->getNbTriangles()==0)
    {
        serr << "ERROR(TriangularTensorMassForceField): object must have a Triangular Set Topology."<<sendl;
        return;
    }
    updateLameCoefficients();

    helper::vector<EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

    /// prepare to store info in the edge array
    edgeInf.resize(_topology->getNbEdges());

    if (_initialPoints.size() == 0)
    {
        // get restPosition
        VecCoord& p = *this->mstate->getX0();
        _initialPoints=p;
    }

    int i;
    // set edge tensor to 0
    for (i=0; i<_topology->getNbEdges(); ++i)
    {
        TriangularTMEdgeCreationFunction(i, (void*) this, edgeInf[i],
                _topology->getEdge(i),  (const sofa::helper::vector< unsigned int > )0,
                (const sofa::helper::vector< double >)0);
    }
    // create edge tensor by calling the triangle creation function
    sofa::helper::vector<unsigned int> triangleAdded;
    for (i=0; i<_topology->getNbTriangles(); ++i)
        triangleAdded.push_back(i);
    TriangularTMTriangleCreationFunction(triangleAdded,(void*) this,
            edgeInf);


    edgeInfo.setCreateFunction(TriangularTMEdgeCreationFunction);
    edgeInfo.setCreateTriangleFunction(TriangularTMTriangleCreationFunction);
    edgeInfo.setDestroyTriangleFunction(TriangularTMTriangleDestructionFunction);
    edgeInfo.setCreateParameter( (void *) this );
    edgeInfo.setDestroyParameter( (void *) this );

    edgeInfo.endEdit();
}


template <class DataTypes>
double TriangularTensorMassForceField<DataTypes>::getPotentialEnergy(const VecCoord& /*x*/)
{
    serr<<"TriangularTensorMassForceField::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}
template <class DataTypes>
void TriangularTensorMassForceField<DataTypes>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& /*v*/)
{
    unsigned int i,v0,v1;
    unsigned int nbEdges=_topology->getNbEdges();

    EdgeRestInformation *einfo;

    helper::vector<EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

    Deriv force;
    Coord dp0,dp1,dp;


    for(i=0; i<nbEdges; i++ )
    {
        einfo=&edgeInf[i];
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];
        dp0=x[v0]-_initialPoints[v0];
        dp1=x[v1]-_initialPoints[v1];
        dp = dp1-dp0;

        f[v1]+=einfo->DfDx*dp;
        f[v0]-=einfo->DfDx.transposeMultiply(dp);
    }

    edgeInfo.endEdit();
}


template <class DataTypes>
void TriangularTensorMassForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx)
{
    unsigned int v0,v1;
    int nbEdges=_topology->getNbEdges();

    EdgeRestInformation *einfo;

    helper::vector<EdgeRestInformation>& edgeInf = *(edgeInfo.beginEdit());

    Deriv force;
    Coord dp0,dp1,dp;

    for(int i=0; i<nbEdges; i++ )
    {
        einfo=&edgeInf[i];
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];
        dp0=dx[v0];
        dp1=dx[v1];
        dp = dp1-dp0;

        df[v1]+=einfo->DfDx*dp;
        df[v0]-=einfo->DfDx.transposeMultiply(dp);
    }

    edgeInfo.endEdit();
}


template<class DataTypes>
void TriangularTensorMassForceField<DataTypes>::updateLameCoefficients()
{
    lambda= f_youngModulus.getValue()*f_poissonRatio.getValue()/(1-f_poissonRatio.getValue()*f_poissonRatio.getValue());
    mu = f_youngModulus.getValue()*(1-f_poissonRatio.getValue())/(1-f_poissonRatio.getValue()*f_poissonRatio.getValue());
//	serr << "initialized Lame coef : lambda=" <<lambda<< " mu="<<mu<<sendl;
}


template<class DataTypes>
void TriangularTensorMassForceField<DataTypes>::draw()
{
    int i;
    if (!getContext()->getShowForceFields()) return;
    if (!this->mstate) return;

    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    VecCoord& x = *this->mstate->getX();
    int nbTriangles=_topology->getNbTriangles();

    glDisable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);
    for(i=0; i<nbTriangles; ++i)
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
