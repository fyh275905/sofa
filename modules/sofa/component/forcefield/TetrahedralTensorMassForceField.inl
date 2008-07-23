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
#include <sofa/component/forcefield/TetrahedralTensorMassForceField.h>
#include <fstream> // for reading the file
#include <iostream> //for debugging
#include <sofa/helper/gl/template.h>
#include <sofa/component/topology/TetrahedronData.inl>
#include <sofa/component/topology/EdgeData.inl>


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using namespace	sofa::component::topology;
using namespace core::componentmodel::topology;

using std::cerr;
using std::cout;
using std::endl;




template< class DataTypes>
void TetrahedralTensorMassForceField<DataTypes>::TetrahedralTMEdgeCreationFunction(int /*edgeIndex*/, void* param, EdgeRestInformation &ei,
        const Edge& ,  const helper::vector< unsigned int > &,
        const helper::vector< double >&)
{
    TetrahedralTensorMassForceField<DataTypes> *ff= (TetrahedralTensorMassForceField<DataTypes> *)param;
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
void TetrahedralTensorMassForceField<DataTypes>::TetrahedralTMTetrahedronCreationFunction (const helper::vector<unsigned int> &tetrahedronAdded,
        void* param, helper::vector<EdgeRestInformation> &edgeData)
{
    TetrahedralTensorMassForceField<DataTypes> *ff= (TetrahedralTensorMassForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int i,j,k,l,u,v;

        typename DataTypes::Real val1,volume;
        typename DataTypes::Real lambda=ff->getLambda();
        typename DataTypes::Real mu=ff->getMu();
        typename DataTypes::Real lambdastar, mustar;
        typename DataTypes::Coord point[4],shapeVector[4];

        const typename DataTypes::VecCoord *restPosition=ff->mstate->getX0();

        for (i=0; i<tetrahedronAdded.size(); ++i)
        {

            /// get a reference on the edge set of the ith added tetrahedron
            const TetrahedronEdges &te= ff->_topology->getEdgeTetraShell(tetrahedronAdded[i]);
            ///get a reference on the vertex set of the ith added tetrahedron
            const Tetrahedron &t= ff->_topology->getTetra(tetrahedronAdded[i]);
            // store points
            for(j=0; j<4; ++j)
                point[j]=(*restPosition)[t[j]];
            /// compute 6 times the rest volume
            volume=dot(cross(point[1]-point[0],point[2]-point[0]),point[0]-point[3]);
            // store shape vectors
            for(j=0; j<4; ++j)
            {
                if ((j%2)==0)
                    shapeVector[j]=cross(point[(j+2)%4] - point[(j+1)%4],point[(j+3)%4] - point[(j+1)%4])/volume;
                else
                    shapeVector[j]= -cross(point[(j+2)%4] - point[(j+1)%4],point[(j+3)%4] - point[(j+1)%4])/volume;
            }

            lambdastar=lambda*fabs(volume)/6;
            mustar=mu*fabs(volume)/6;


            for(j=0; j<6; ++j)
            {
                /// local indices of the edge
                k = ff->_topology->getLocalTetrahedronEdges(j)[0];
                l = ff->_topology->getLocalTetrahedronEdges(j)[1];

                Mat3 &m=edgeData[te[j]].DfDx;

                val1= dot(shapeVector[k],shapeVector[l])*mustar;
                // print if obtuse tetrahedron along that edge
                if (ff->f_printLog.getValue())
                {
                    if (val1<0)
                        std::cerr<<"negative cotangent["<<tetrahedronAdded[i]<<"]["<<j<<"]"<<std::endl;
                }

                if (ff->_topology->getEdge(te[j])[0]!=t[l])
                {
                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[u][v]+= lambdastar*shapeVector[l][u]*shapeVector[k][v]+mustar*shapeVector[k][u]*shapeVector[l][v];
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
                            m[v][u]+= lambdastar*shapeVector[l][u]*shapeVector[k][v]+mustar*shapeVector[k][u]*shapeVector[l][v];
                        }
                        m[u][u]+=val1;
                    }
                }


            }
        }

    }
}

template< class DataTypes>
void TetrahedralTensorMassForceField<DataTypes>::TetrahedralTMTetrahedronDestructionFunction (const helper::vector<unsigned int> &tetrahedronRemoved,
        void* param, helper::vector<EdgeRestInformation> &edgeData)
{
    TetrahedralTensorMassForceField<DataTypes> *ff= (TetrahedralTensorMassForceField<DataTypes> *)param;
    if (ff)
    {

        unsigned int i,j,k,l,u,v;

        typename DataTypes::Real val1,volume;
        typename DataTypes::Real lambda=ff->getLambda();
        typename DataTypes::Real mu=ff->getMu();
        typename DataTypes::Real lambdastar, mustar;
        typename DataTypes::Coord point[4],shapeVector[4];

        const typename DataTypes::VecCoord *restPosition=ff->mstate->getX0();

        for (i=0; i<tetrahedronRemoved.size(); ++i)
        {

            /// get a reference on the edge set of the ith added tetrahedron
            const TetrahedronEdges &te= ff->_topology->getEdgeTetraShell(tetrahedronRemoved[i]);
            ///get a reference on the vertex set of the ith added tetrahedron
            const Tetrahedron &t= ff->_topology->getTetra(tetrahedronRemoved[i]);
            // store points
            for(j=0; j<4; ++j)
                point[j]=(*restPosition)[t[j]];
            /// compute 6 times the rest volume
            volume=dot(cross(point[1]-point[0],point[2]-point[0]),point[0]-point[3]);
            // store shape vectors
            for(j=0; j<4; ++j)
            {
                if ((j%2)==0)
                    shapeVector[j]=cross(point[(j+2)%4] - point[(j+1)%4],point[(j+3)%4] - point[(j+1)%4])/volume;
                else
                    shapeVector[j]= -cross(point[(j+2)%4] - point[(j+1)%4],point[(j+3)%4] - point[(j+1)%4])/volume;
            }

            lambdastar=lambda*fabs(volume)/6;
            mustar=mu*fabs(volume)/6;


            for(j=0; j<6; ++j)
            {
                /// local indices of the edge
                k = ff->_topology->getLocalTetrahedronEdges(j)[0];
                l = ff->_topology->getLocalTetrahedronEdges(j)[1];

                Mat3 &m=edgeData[te[j]].DfDx;

                val1= dot(shapeVector[k],shapeVector[l])*mustar;
                // print if obtuse tetrahedron along that edge
                if (ff->f_printLog.getValue())
                {
                    if (val1<0)
                        std::cerr<<"negative cotangent["<<tetrahedronRemoved[i]<<"]["<<j<<"]"<<std::endl;
                }

                if (ff->_topology->getEdge(te[j])[0]!=t[l])
                {
                    for (u=0; u<3; ++u)
                    {
                        for (v=0; v<3; ++v)
                        {
                            m[u][v]-= lambdastar*shapeVector[l][u]*shapeVector[k][v]+mustar*shapeVector[k][u]*shapeVector[l][v];
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
                            m[v][u]-= lambdastar*shapeVector[l][u]*shapeVector[k][v]+mustar*shapeVector[k][u]*shapeVector[l][v];
                        }
                        m[u][u]-=val1;
                    }
                }


            }

        }

    }
}


template <class DataTypes> TetrahedralTensorMassForceField<DataTypes>::TetrahedralTensorMassForceField()
    : _initialPoints(0)
    , updateMatrix(true)
    , f_poissonRatio(initData(&f_poissonRatio,(Real)0.3,"poissonRatio","Poisson ratio in Hooke's law"))
    , f_youngModulus(initData(&f_youngModulus,(Real)1000.,"youngModulus","Young modulus in Hooke's law"))
    , lambda(0)
    , mu(0)
{
}

template <class DataTypes> void TetrahedralTensorMassForceField<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    edgeInfo.handleTopologyEvents(itBegin,itEnd);
}

template <class DataTypes> TetrahedralTensorMassForceField<DataTypes>::~TetrahedralTensorMassForceField()
{

}

template <class DataTypes> void TetrahedralTensorMassForceField<DataTypes>::init()
{
    std::cerr << "initializing TetrahedralTensorMassForceField" << std::endl;
    this->Inherited::init();

    _topology = getContext()->getMeshTopology();

    sofa::component::topology::TetrahedronSetTopologyContainer* tetraCont;
    this->getContext()->get(tetraCont);

    if ((tetraCont==0) || (_topology->getNbTetras()==0))
    {
        std::cerr << "ERROR(TetrahedralTensorMassForceField): object must have a Tetrahedral Set Topology.\n";
        return;
    }
    updateLameCoefficients();

    /// prepare to store info in the edge array
    edgeInfo.resize(_topology->getNbEdges());

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
        TetrahedralTMEdgeCreationFunction(i, (void*) this, edgeInfo[i],
                _topology->getEdge(i),  (const std::vector< unsigned int > )0,
                (const std::vector< double >)0);
    }
    // create edge tensor by calling the tetrahedron creation function
    std::vector<unsigned int> tetrahedronAdded;
    for (i=0; i<_topology->getNbTetras(); ++i)
        tetrahedronAdded.push_back(i);
    TetrahedralTMTetrahedronCreationFunction(tetrahedronAdded,(void*) this,
            edgeInfo);


    edgeInfo.setCreateFunction(TetrahedralTMEdgeCreationFunction);
    edgeInfo.setCreateTetrahedronFunction(TetrahedralTMTetrahedronCreationFunction);
    edgeInfo.setDestroyTetrahedronFunction(TetrahedralTMTetrahedronDestructionFunction);
    edgeInfo.setCreateParameter( (void *) this );
    edgeInfo.setDestroyParameter( (void *) this );

}


template <class DataTypes>
double TetrahedralTensorMassForceField<DataTypes>::getPotentialEnergy(const VecCoord& /*x*/)
{
    std::cerr<<"TetrahedralTensorMassForceField::getPotentialEnergy-not-implemented !!!"<<endl;
    return 0;
}
template <class DataTypes>
void TetrahedralTensorMassForceField<DataTypes>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& /*v*/)
{
    unsigned int v0,v1;
    int nbEdges=_topology->getNbEdges();

    EdgeRestInformation *einfo;

    Deriv force;
    Coord dp0,dp1,dp;


    for(int i=0; i<nbEdges; i++ )
    {
        einfo=&edgeInfo[i];
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];
        dp0=x[v0]-_initialPoints[v0];
        dp1=x[v1]-_initialPoints[v1];
        dp = dp1-dp0;

        f[v1]+=einfo->DfDx*dp;
        f[v0]-=einfo->DfDx.transposeMultiply(dp);
    }

}


template <class DataTypes>
void TetrahedralTensorMassForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx)
{
    unsigned int v0,v1;
    int nbEdges=_topology->getNbEdges();

    EdgeRestInformation *einfo;


    Deriv force;
    Coord dp0,dp1,dp;

    for(int i=0; i<nbEdges; i++ )
    {
        einfo=&edgeInfo[i];
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];
        dp0=dx[v0];
        dp1=dx[v1];
        dp = dp1-dp0;

        df[v1]+=einfo->DfDx*dp;
        df[v0]-=einfo->DfDx.transposeMultiply(dp);
    }

}


template<class DataTypes>
void TetrahedralTensorMassForceField<DataTypes>::updateLameCoefficients()
{
    lambda= f_youngModulus.getValue()*f_poissonRatio.getValue()/((1-2*f_poissonRatio.getValue())*(1+f_poissonRatio.getValue()));
    mu = f_youngModulus.getValue()/(2*(1+f_poissonRatio.getValue()));
//	std::cerr << "initialized Lame coef : lambda=" <<lambda<< " mu="<<mu<<std::endl;
}


template<class DataTypes>
void TetrahedralTensorMassForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!this->mstate) return;

    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// 	VecCoord& x = *this->mstate->getX();
// 	int nbTriangles=_topology->getNbTriangles();

    /*
    	glDisable(GL_LIGHTING);

    	glBegin(GL_TRIANGLES);
    	for(i=0;i<nbTriangles; ++i)
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

    */
    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

} // namespace forcefield

} // namespace Components

} // namespace Sofa
