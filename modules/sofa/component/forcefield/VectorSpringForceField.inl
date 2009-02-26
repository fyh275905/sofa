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
#ifndef SOFA_COMPONENT_FORCEFIELD_VECTORSPRINGFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_VECTORSPRINGFORCEFIELD_INL

#include "VectorSpringForceField.h"
#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/component/topology/EdgeData.inl>
#include <sofa/component/topology/TopologyChangedEvent.h>
#include <sofa/helper/system/config.h>
#include <sofa/helper/system/gl.h>
#include <sofa/simulation/common/Simulation.h>
#include <assert.h>
#include <iostream>



namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
void VectorSpringForceField<DataTypes>::springCreationFunction(int /*index*/,
        void* param, Spring& t,
        const topology::Edge& e,
        const sofa::helper::vector< unsigned int > &ancestors,
        const sofa::helper::vector< double >& coefs)
{
    VectorSpringForceField<DataTypes> *ff= static_cast<VectorSpringForceField<DataTypes> *>(param);
    if (ff)
    {


        //EdgeSetGeometryAlgorithms<DataTypes> *ga=topology->getEdgeSetGeometryAlgorithms();
        //t.restLength=ga->computeRestEdgeLength(index);
        const typename DataTypes::VecCoord& x0 = *ff->getObject1()->getX0();
        t.restVector = x0[e[1]] - x0[e[0]];
        if (ancestors.size()>0)
        {
            t.kd=t.ks=0;
//            const topology::EdgeData<Spring> &sa=ff->getSpringArray();
            const helper::vector<Spring> &sa=ff->getSpringArray().getValue();
            unsigned int i;
            for (i=0; i<ancestors.size(); ++i)
            {
                t.kd+=(typename DataTypes::Real)(sa[i].kd*coefs[i]);
                t.ks+=(typename DataTypes::Real)(sa[i].ks*coefs[i]);
            }
        }
        else
        {
            t.kd=ff->getStiffness();
            t.ks=ff->getViscosity();
        }

    }
}

template <class DataTypes>
class VectorSpringForceField<DataTypes>::Loader : public sofa::helper::io::MassSpringLoader
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    VectorSpringForceField<DataTypes>* dest;
    Loader(VectorSpringForceField<DataTypes>* dest) : dest(dest) {}
    virtual void addVectorSpring(int m1, int m2, SReal ks, SReal kd, SReal /*initpos*/, SReal restx, SReal resty, SReal restz)
    {
        dest->addSpring(m1,m2,ks,kd,Coord((Real)restx,(Real)resty,(Real)restz));
    }
    virtual void setNumSprings(int /*n*/)
    {
        //dest->resizeArray((unsigned int )n);
    }

};

template <class DataTypes>
bool VectorSpringForceField<DataTypes>::load(const char *filename)
{
    if (filename && filename[0])
    {
        Loader loader(this);
        return loader.load(filename);
    }
    else return false;
}

template <class DataTypes>
void VectorSpringForceField<DataTypes>::resizeArray(unsigned int n)
{
    helper::vector<Spring>& springArrayData = *(springArray.beginEdit());
    springArrayData.resize(n);
    springArray.endEdit();
}

template <class DataTypes>
void VectorSpringForceField<DataTypes>::addSpring(int m1, int m2, SReal ks, SReal kd, Coord restVector)
{
    helper::vector<Spring>& springArrayData = *(springArray.beginEdit());

    if (useTopology && _topology)
    {
        int e=_topology->getEdgeIndex((unsigned int)m1,(unsigned int)m2);
        if (e>=0)
            springArrayData[e]=Spring((Real)ks,(Real)kd,restVector);
    }
    else
    {
        springArrayData.push_back(Spring((Real)ks, (Real)kd, restVector));
        edgeArray.push_back(topology::Edge(m1,m2));
    }
}

template <class DataTypes>
VectorSpringForceField<DataTypes>::VectorSpringForceField(MechanicalState* _object)
    : Inherit(_object, _object)
    , m_potentialEnergy( 0.0 ), useTopology( false )
    , m_filename( initData(&m_filename,std::string(""),"filename","File name from which the spring informations are loaded") )
    , m_stiffness( initData(&m_stiffness,1.0,"stiffness","Default edge stiffness used in absence of file information") )
    , m_viscosity( initData(&m_viscosity,1.0,"viscosity","Default edge viscosity used in absence of file information") )
{
    springArray.setCreateFunction(springCreationFunction);
    springArray.setCreateParameter( (void *) this );
}

template <class DataTypes>
VectorSpringForceField<DataTypes>::VectorSpringForceField(MechanicalState* _object1, MechanicalState* _object2)
    : Inherit(_object1, _object2)
    , m_potentialEnergy( 0.0 ), useTopology( false )
    , m_filename( initData(&m_filename,std::string(""),"filename","File name from which the spring informations are loaded") )
    , m_stiffness( initData(&m_stiffness,1.0,"stiffness","Default edge stiffness used in absence of file information") )
    , m_viscosity( initData(&m_viscosity,1.0,"viscosity","Default edge viscosity used in absence of file information") )
{
    springArray.setCreateFunction(springCreationFunction);
    springArray.setCreateParameter( (void *) this );
}

template <class DataTypes>
void VectorSpringForceField<DataTypes>::init()
{
    _topology = this->getContext()->getMeshTopology();
    this->getContext()->get(edgeGeo);
    this->getContext()->get(edgeMod);

    this->Inherit::init();
}

template <class DataTypes>
void VectorSpringForceField<DataTypes>::bwdInit()
{
    this->Inherit::bwdInit();
    helper::vector<Spring>& springArrayData = *(springArray.beginEdit());

    if (springArrayData.empty())
    {
        if (!m_filename.getValue().empty())
        {
            // load the springs from a file
            load(( const char *)(m_filename.getValue().c_str()));
            return;
        }

        if (_topology != NULL)
        {
            // create springs based on the mesh topology
            useTopology = true;
            createDefaultSprings();
            f_listening.setValue(true);
        }
        else
        {
            int n = this->mstate1->getSize();
            sout << "VectorSpringForceField: linking "<<n<<" pairs of points." << sendl;
            springArrayData.resize(n);
            edgeArray.resize(n);
            for (int i=0; i<n; ++i)
            {
                edgeArray[i][0] = i;
                edgeArray[i][1] = i;
                springArrayData[i].ks=(Real)m_stiffness.getValue();
                springArrayData[i].kd=(Real)m_viscosity.getValue();
                springArrayData[i].restVector = Coord();
            }
        }
        springArray.endEdit();
    }
}

template <class DataTypes>
void VectorSpringForceField<DataTypes>::createDefaultSprings()
{
    sout << "Creating "<< _topology->getNbEdges() <<" Vector Springs from EdgeSetTopology"<<sendl;

    helper::vector<Spring>& springArrayData = *(springArray.beginEdit());

    springArrayData.resize(_topology->getNbEdges());
    //EdgeLengthArrayInterface<Real,DataTypes> elai(springArray);
    //edgeGEO->computeEdgeLength(elai);
    const VecCoord& x0 = *this->mstate1->getX0();
    int i;
    for (i=0; i<_topology->getNbEdges(); ++i)
    {
        springArrayData[i].ks=(Real)m_stiffness.getValue();
        springArrayData[i].kd=(Real)m_viscosity.getValue();
        springArrayData[i].restVector = x0[_topology->getEdge(i)[1]]-x0[_topology->getEdge(i)[0]];
    }

    springArray.endEdit();
}
template<class DataTypes>
void VectorSpringForceField<DataTypes>::handleEvent( Event* e )
{
    if (useTopology)
    {
        if( sofa::core::objectmodel::KeypressedEvent* ke = dynamic_cast<sofa::core::objectmodel::KeypressedEvent*>( e ) )
        {
            /// handle ctrl+d key
            if (ke->getKey()=='D')
            {
                if (_topology->getNbEdges()>12)
                {

                    sofa::helper::vector<unsigned int> edgeArray;
                    edgeArray.push_back(12);
                    edgeMod->removeEdges(edgeArray);
                }
                //            edgeMod->splitEdges(edgeArray);
            }
        }
        else
        {
            sofa::component::topology::TopologyChangedEvent *tce=dynamic_cast<sofa::component::topology::TopologyChangedEvent *>(e);
            /// test that the event is a change of topology and that it
            if ((tce) && edgeMod /*&& (tce->getMeshTopology()== _topology )*/)
            {
                std::list<const sofa::core::componentmodel::topology::TopologyChange *>::const_iterator itBegin=_topology->firstChange();
                std::list<const sofa::core::componentmodel::topology::TopologyChange *>::const_iterator itEnd=_topology->lastChange();
                /// Topological events are handled by the EdgeData structure
                springArray.handleTopologyEvents(itBegin,itEnd);
            }
        }
    }
}

template<class DataTypes>
//void VectorSpringForceField<DataTypes>::addForce(VecDeriv& f, const VecCoord& p, const VecDeriv& v)
void VectorSpringForceField<DataTypes>::addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2)
{
    //assert(this->mstate);
    m_potentialEnergy = 0;

    Coord u;

    f1.resize(x1.size());
    f2.resize(x2.size());

    helper::vector<Spring>& springArrayData = *(springArray.beginEdit());

    if(useTopology)
    {

        Deriv relativeVelocity,force;
        for (int i=0; i<_topology->getNbEdges(); i++)
        {
            const topology::Edge &e=_topology->getEdge(i);
            const Spring &s=springArrayData[i];
            // paul---------------------------------------------------------------
            Deriv current_direction = x2[e[1]]-x1[e[0]];
            Deriv squash_vector = current_direction - s.restVector;
            Deriv relativeVelocity = v2[e[1]]-v1[e[0]];
            force = (squash_vector * s.ks) + (relativeVelocity * s.kd);

            f1[e[0]]+=force;
            f2[e[1]]-=force;
        }

    }
    else
    {

        Deriv relativeVelocity,force;
        for (unsigned int i=0; i<edgeArray.size(); i++)
        {
            const topology::Edge &e=edgeArray[i];
            const Spring &s=springArrayData[i];
            // paul---------------------------------------------------------------
            Deriv current_direction = x2[e[1]]-x1[e[0]];
            Deriv squash_vector = current_direction - s.restVector;
            Deriv relativeVelocity = v2[e[1]]-v1[e[0]];
            force = (squash_vector * s.ks) + (relativeVelocity * s.kd);

            f1[e[0]]+=force;
            f2[e[1]]-=force;
        }
    }
    springArray.endEdit();
}

template<class DataTypes>
//void VectorSpringForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx)
void VectorSpringForceField<DataTypes>::addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2, double kFactor, double /*bFactor*/)
{
    Deriv dforce,d;

    df1.resize(dx1.size());
    df2.resize(dx2.size());

    helper::vector<Spring>& springArrayData = *(springArray.beginEdit());

    if(useTopology)
    {

        for (int i=0; i<_topology->getNbEdges(); i++)
        {
            const topology::Edge &e=_topology->getEdge(i);
            const Spring &s=springArrayData[i];
            d = dx2[e[1]]-dx1[e[0]];
            dforce = d*(s.ks*kFactor);
            df1[e[0]]+=dforce;
            df2[e[1]]-=dforce;
        }

    }
    else
    {

        for (unsigned int i=0; i<edgeArray.size(); i++)
        {
            const topology::Edge &e=edgeArray[i];
            const Spring &s=springArrayData[i];
            d = dx2[e[1]]-dx1[e[0]];
            dforce = d*(s.ks*kFactor);
            df1[e[0]]+=dforce;
            df2[e[1]]-=dforce;
        }
    }
    springArray.endEdit();
}

template<class DataTypes>
void VectorSpringForceField<DataTypes>::draw()
{
    if (!((this->mstate1 == this->mstate2)?getContext()->getShowForceFields():getContext()->getShowInteractionForceFields()))
        return;
    //const VecCoord& p = *this->mstate->getX();
    const VecCoord& x1 = *this->mstate1->getX();
    const VecCoord& x2 = *this->mstate2->getX();


    std::vector< Vector3 > points;
    if(useTopology)
    {
        for (unsigned int i=0; i<springArray.getValue().size(); i++)
        {
            const topology::Edge &e=_topology->getEdge(i);
            //const Spring &s=springArray[i];

            points.push_back(Vector3(x1[e[0]]));
            points.push_back(Vector3(x2[e[1]]));
        }

    }
    else
    {

        for (unsigned int i=0; i<springArray.getValue().size(); i++)
        {
            const topology::Edge &e=edgeArray[i];
            //const Spring &s=springArray[i];

            points.push_back(Vector3(x1[e[0]]));
            points.push_back(Vector3(x2[e[1]]));
        }
    }
    simulation::getSimulation()->DrawUtility.drawLines(points, 3, Vec<4,float>(0,1,1,1));
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
