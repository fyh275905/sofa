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

#include <sofa/component/collision/TriangleLocalMinDistanceFilter.h>
#include <sofa/component/topology/EdgeData.inl>
#include <sofa/component/topology/PointData.inl>
#include <sofa/component/topology/TriangleData.inl>

#include <sofa/core/ObjectFactory.h>


namespace sofa
{

namespace component
{

namespace collision
{

void TriangleInfo::buildFilter(unsigned int tri_index)
{
    if (isRigid())
    {
        std::cout<<"TODO in TriangleInfo::buildFilter"<<std::endl;
        // TODO : supprimer ! : a priori, on ne passe plus dans cette fonction quand on a un objet rigide !
    }
    else
    {

        sofa::core::topology::BaseMeshTopology* bmt = this->base_mesh_topology;
        const Triangle &t =  bmt->getTriangle(tri_index);

        const Vector3 &pt1 = (*this->position_filtering)[t[0]];
        const Vector3 &pt2 = (*this->position_filtering)[t[1]];
        const Vector3 &pt3 = (*this->position_filtering)[t[2]];

        m_normal = cross(pt2-pt1, pt3-pt1);
    }

    setValid();
}



bool TriangleInfo::validate(const unsigned int tri_index, const defaulttype::Vector3 &PQ)
{
    //std::cout<<"TriangleInfo::validate on tri "<<tri_index<<"is called"<<std::endl;
    if (isValid())
    {
        //std::cout<<" is Valid !"<<std::endl;
        return ( (m_normal * PQ) >= 0.0 );
    }
    else
    {
        //std::cout<<" not valid => build ------------------------ for triangle "<< tri_index <<std::endl;
        buildFilter(tri_index);
        return validate(tri_index, PQ);
    }
}



void TriangleLocalMinDistanceFilter::init()
{
    core::topology::BaseMeshTopology *bmt = getContext()->getMeshTopology();
    std::cout<<"Mesh Topology found :"<<bmt->getName()<<std::endl;
    component::container::MechanicalObject<Vec3dTypes>*  mstateVec3d= dynamic_cast<component::container::MechanicalObject<Vec3dTypes>*>(getContext()->getMechanicalState());


    if(mstateVec3d == NULL)
    {
        serr<<"WARNING: init failed for TriangleLocalMinDistanceFilter no mstateVec3d found"<<sendl;
    }

    if (bmt != 0)
    {


        m_pointInfo.setCreateFunction(LMDFilterPointCreationFunction);
        m_pointInfo.setCreateParameter((void *) this);
        helper::vector< PointInfo >& pInfo = *(m_pointInfo.beginEdit());
        pInfo.resize(bmt->getNbPoints());
        int i;
        for (i=0; i<bmt->getNbPoints(); i++)
        {
            pInfo[i].setLMDFilters(this);
            pInfo[i].setBaseMeshTopology(bmt);
            pInfo[i].setPositionFiltering(mstateVec3d->getX());
        }
        m_pointInfo.endEdit();




        m_lineInfo.setCreateFunction(LMDFilterLineCreationFunction);
        m_lineInfo.setCreateParameter((void *) this);
        helper::vector< LineInfo >& lInfo = *(m_lineInfo.beginEdit());
        lInfo.resize(bmt->getNbEdges());
        for (i=0; i<bmt->getNbEdges(); i++)
        {
            lInfo[i].setLMDFilters(this);
            lInfo[i].setBaseMeshTopology(bmt);
            lInfo[i].setPositionFiltering(mstateVec3d->getX());
        }
        m_lineInfo.endEdit();





        m_triangleInfo.setCreateFunction(LMDFilterTriangleCreationFunction);
        m_triangleInfo.setCreateParameter((void *) this);
        helper::vector< TriangleInfo >& tInfo = *(m_triangleInfo.beginEdit());
        tInfo.resize(bmt->getNbTriangles());
        for (i=0; i<bmt->getNbTriangles(); i++)
        {
            tInfo[i].setLMDFilters(this);
            tInfo[i].setBaseMeshTopology(bmt);
            tInfo[i].setPositionFiltering(mstateVec3d->getX());
        }
        m_triangleInfo.endEdit();
        std::cout<<"create m_pointInfo, m_lineInfo, m_triangleInfo" <<std::endl;
    }
}



void TriangleLocalMinDistanceFilter::handleTopologyChange()
{
    core::topology::BaseMeshTopology *bmt = getContext()->getMeshTopology();

    assert(bmt != 0);

    std::list< const core::topology::TopologyChange * >::const_iterator itBegin = bmt->firstChange();
    std::list< const core::topology::TopologyChange * >::const_iterator itEnd = bmt->lastChange();

    m_pointInfo.handleTopologyEvents(itBegin, itEnd);
    m_lineInfo.handleTopologyEvents(itBegin, itEnd);
    m_triangleInfo.handleTopologyEvents(itBegin, itEnd);
}



void TriangleLocalMinDistanceFilter::LMDFilterPointCreationFunction(int, void *param, PointInfo &pInfo, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >&)
{
    std::cout<<"LMDFilterPointCreationFunction is called "<<std::endl;
    const PointLocalMinDistanceFilter *pLMDFilter = static_cast< const PointLocalMinDistanceFilter * >(param);
    pInfo.setLMDFilters(pLMDFilter);
    sofa::core::topology::BaseMeshTopology * bmt = (sofa::core::topology::BaseMeshTopology *)pLMDFilter->getContext()->getTopology();
    pInfo.setBaseMeshTopology(bmt);
    /////// TODO : template de la classe
    component::container::MechanicalObject<Vec3dTypes>*  mstateVec3d= dynamic_cast<component::container::MechanicalObject<Vec3dTypes>*>(pLMDFilter->getContext()->getMechanicalState());
    if(mstateVec3d != NULL)
    {
        pInfo.setPositionFiltering(mstateVec3d->getX());
    }

}



void TriangleLocalMinDistanceFilter::LMDFilterLineCreationFunction(int, void *param, LineInfo &lInfo, const topology::Edge&, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >&)
{
    std::cout<<"LMDFilterLineCreationFunction is called "<<std::endl;
    const LineLocalMinDistanceFilter *lLMDFilter = static_cast< const LineLocalMinDistanceFilter * >(param);
    lInfo.setLMDFilters(lLMDFilter);
    sofa::core::topology::BaseMeshTopology * bmt = (sofa::core::topology::BaseMeshTopology *)lLMDFilter->getContext()->getTopology();
    lInfo.setBaseMeshTopology(bmt);
    /////// TODO : template de la classe
    component::container::MechanicalObject<Vec3dTypes>*  mstateVec3d= dynamic_cast<component::container::MechanicalObject<Vec3dTypes>*>(lLMDFilter->getContext()->getMechanicalState());
    if(mstateVec3d != NULL)
    {
        lInfo.setPositionFiltering(mstateVec3d->getX());
    }
}



void TriangleLocalMinDistanceFilter::LMDFilterTriangleCreationFunction(int, void *param, TriangleInfo &tInfo, const topology::Triangle&, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >&)
{
    std::cout<<"LMDFilterTriangleCreationFunction is called "<<std::endl;
    const TriangleLocalMinDistanceFilter *tLMDFilter = static_cast< const TriangleLocalMinDistanceFilter * >(param);
    tInfo.setLMDFilters(tLMDFilter);

    sofa::core::topology::BaseMeshTopology * bmt = (sofa::core::topology::BaseMeshTopology *)tLMDFilter->getContext()->getTopology();
    tInfo.setBaseMeshTopology(bmt);
    /////// TODO : template de la classe
    component::container::MechanicalObject<Vec3dTypes>*  mstateVec3d= dynamic_cast<component::container::MechanicalObject<Vec3dTypes>*>(tLMDFilter->getContext()->getMechanicalState());
    if(mstateVec3d != NULL)
    {
        tInfo.setPositionFiltering(mstateVec3d->getX());
    }
}



SOFA_DECL_CLASS(TriangleLocalMinDistanceFilter)

int TriangleLocalMinDistanceFilterClass = core::RegisterObject("This class manages Triangle collision models cones filters computations and updates.")
        .add< TriangleLocalMinDistanceFilter >()
        ;

} // namespace collision

} // namespace component

} // namespace sofa
