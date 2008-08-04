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
#include <sofa/component/topology/Triangle2EdgeTopologicalMapping.h>

#include <sofa/core/ObjectFactory.h>

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>

#include <sofa/component/topology/EdgeSetTopologyContainer.h>
#include <sofa/component/topology/EdgeSetTopologyModifier.h>
#include <sofa/component/topology/EdgeSetTopologyChange.h>

#include <sofa/component/topology/TriangleSetTopologyContainer.h>
#include <sofa/component/topology/TriangleSetTopologyModifier.h>
#include <sofa/component/topology/TriangleSetTopologyChange.h>

#include <sofa/component/topology/PointSetTopologyChange.h>

#include <sofa/defaulttype/Vec.h>
#include <map>
#include <sofa/defaulttype/VecTypes.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;

using namespace sofa::component::topology;
using namespace sofa::core::componentmodel::topology;

/// Input Topology
typedef BaseMeshTopology In;
/// Output Topology
typedef BaseMeshTopology Out;

SOFA_DECL_CLASS(Triangle2EdgeTopologicalMapping)

// Register in the Factory
int Triangle2EdgeTopologicalMappingClass = core::RegisterObject("Special case of mapping where TriangleSetTopology is converted to EdgeSetTopology")
        .add< Triangle2EdgeTopologicalMapping >()

        ;

Triangle2EdgeTopologicalMapping::Triangle2EdgeTopologicalMapping(In* from, Out* to)
    :
    fromModel(from), toModel(to),
    object1(initData(&object1, std::string("../.."), "object1", "First object to map")),
    object2(initData(&object2, std::string(".."), "object2", "Second object to map"))
{
}


Triangle2EdgeTopologicalMapping::~Triangle2EdgeTopologicalMapping()
{
}

In* Triangle2EdgeTopologicalMapping::getFromModel()
{
    return this->fromModel;
}

Out* Triangle2EdgeTopologicalMapping::getToModel()
{
    return this->toModel;
}

objectmodel::BaseObject* Triangle2EdgeTopologicalMapping::getFrom()
{
    return this->fromModel;
}

objectmodel::BaseObject* Triangle2EdgeTopologicalMapping::getTo()
{
    return this->toModel;
}


void Triangle2EdgeTopologicalMapping::setModels(In* from, Out* to)
{
    this->fromModel = from;
    this->toModel = to;
}


void Triangle2EdgeTopologicalMapping::init()
{
    std::cout << "INFO_print : init Triangle2EdgeTopologicalMapping" << std::endl;

    // INITIALISATION of EDGE mesh from TRIANGULAR mesh :

    if (fromModel)
    {

        std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - from = triangle" << std::endl;

        if (toModel)
        {

            std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - to = edge" << std::endl;

            EdgeSetTopologyModifier *to_tstm;
            toModel->getContext()->get(to_tstm);

            const sofa::helper::vector<Edge> &edgeArray=fromModel->getEdges();

            unsigned int nb_visible_edges = 0;

            Loc2GlobVec.clear();
            Glob2LocMap.clear();

            for (unsigned int i=0; i<edgeArray.size(); ++i)
            {

                if (fromModel->getTriangleEdgeShell(i).size()==1)
                {

                    to_tstm->addEdgeProcess(edgeArray[i]);

                    Loc2GlobVec.push_back(i);
                    Glob2LocMap[i]=Loc2GlobVec.size()-1;

                    nb_visible_edges+=1;
                }
            }

            to_tstm->notifyEndingEvent();
        }

    }

}

unsigned int Triangle2EdgeTopologicalMapping::getFromIndex(unsigned int ind)
{

    if(fromModel->getTriangleEdgeShell(ind).size()==1)
    {
        return fromModel->getTriangleEdgeShell(ind)[0];
    }
    else
    {
        return 0;
    }
}

void Triangle2EdgeTopologicalMapping::updateTopologicalMapping()
{

    // INITIALISATION of EDGE mesh from TRIANGULAR mesh :

    if (fromModel)
    {

        EdgeSetTopologyModifier *to_tstm;
        toModel->getContext()->get(to_tstm);

        if (toModel)
        {

            std::list<const TopologyChange *>::const_iterator itBegin=fromModel->firstChange();
            std::list<const TopologyChange *>::const_iterator itEnd=fromModel->lastChange();

            while( itBegin != itEnd )
            {
                TopologyChangeType changeType = (*itBegin)->getChangeType();

                switch( changeType )
                {

                case core::componentmodel::topology::ENDING_EVENT:
                {
                    //std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - ENDING_EVENT" << std::endl;
                    to_tstm->notifyEndingEvent();
                    break;
                }

                case core::componentmodel::topology::EDGESREMOVED:
                {
                    //std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - EDGESREMOVED" << std::endl;

                    int last;
                    int ind_last;

                    last= fromModel->getNbEdges() - 1;

                    const sofa::helper::vector<unsigned int> &tab = ( static_cast< const EdgesRemoved *>( *itBegin ) )->getArray();

                    unsigned int ind_tmp;

                    unsigned int ind_real_last;
                    ind_last=toModel->getNbEdges();

                    for (unsigned int i = 0; i <tab.size(); ++i)
                    {
                        unsigned int k = tab[i];
                        unsigned int ind_k;

                        std::map<unsigned int, unsigned int>::iterator iter_1 = Glob2LocMap.find(k);
                        if(iter_1 != Glob2LocMap.end())
                        {

                            ind_last = ind_last - 1;

                            ind_k = Glob2LocMap[k];
                            ind_real_last = ind_k;

                            std::map<unsigned int, unsigned int>::iterator iter_2 = Glob2LocMap.find(last);
                            if(iter_2 != Glob2LocMap.end())
                            {

                                ind_real_last = Glob2LocMap[last];

                                if((int) k != last)
                                {

                                    Glob2LocMap.erase(Glob2LocMap.find(k));
                                    Glob2LocMap[k] = ind_real_last;

                                    Glob2LocMap.erase(Glob2LocMap.find(last));
                                    Glob2LocMap[last] = ind_k;

                                    ind_tmp = Loc2GlobVec[ind_real_last];
                                    Loc2GlobVec[ind_real_last] = Loc2GlobVec[ind_k];
                                    Loc2GlobVec[ind_k] = ind_tmp;
                                }
                            }

                            if((int) ind_k != ind_last)
                            {

                                Glob2LocMap.erase(Glob2LocMap.find(Loc2GlobVec[ind_last]));
                                Glob2LocMap[Loc2GlobVec[ind_last]] = ind_k;

                                Glob2LocMap.erase(Glob2LocMap.find(Loc2GlobVec[ind_k]));
                                Glob2LocMap[Loc2GlobVec[ind_k]] = ind_last;

                                ind_tmp = Loc2GlobVec[ind_k];
                                Loc2GlobVec[ind_k] = Loc2GlobVec[ind_last];
                                Loc2GlobVec[ind_last] = ind_tmp;

                            }

                            Glob2LocMap.erase(Glob2LocMap.find(Loc2GlobVec[Loc2GlobVec.size() - 1]));
                            Loc2GlobVec.resize( Loc2GlobVec.size() - 1 );

                            sofa::helper::vector< unsigned int > edges_to_remove;
                            edges_to_remove.push_back(ind_k);
                            to_tstm->removeEdges(edges_to_remove, false);

                        }
                        else
                        {

                            std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - Glob2LocMap should have the visible edge " << tab[i] << std::endl;
                            std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - nb edges = " << ind_last << std::endl;
                        }

                        --last;
                    }

                    //to_tstm->propagateTopologicalChanges();

                    break;
                }

                case core::componentmodel::topology::TRIANGLESREMOVED:
                {

                    //std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - TRIANGLESREMOVED" << std::endl;

                    if (fromModel)
                    {

                        const sofa::helper::vector<Triangle> &triangleArray=fromModel->getTriangles();

                        const sofa::helper::vector<unsigned int> &tab = ( static_cast< const TrianglesRemoved *>( *itBegin ) )->getArray();

                        sofa::helper::vector< Edge > edges_to_create;
                        sofa::helper::vector< unsigned int > edgesIndexList;
                        int nb_elems = toModel->getNbEdges();

                        for (unsigned int i = 0; i < tab.size(); ++i)
                        {
                            for (unsigned int j = 0; j < 3; ++j)
                            {
                                unsigned int k = (fromModel->getEdgeTriangleShell(tab[i]))[j];

                                if (fromModel->getTriangleEdgeShell(k).size()!= 2)   // remove as visible the edge indexed by k // ==1
                                {

                                    // do nothing

                                }
                                else   // fromModel->getTriangleEdgeShell(k).size()==2 // add as visible the edge indexed by k
                                {

                                    unsigned int ind_test;
                                    if(tab[i] == fromModel->getTriangleEdgeShell(k)[0])
                                    {

                                        ind_test = fromModel->getTriangleEdgeShell(k)[1];

                                    }
                                    else   // tab[i] == fromModel->getTriangleEdgeShell(k)[1]
                                    {

                                        ind_test = fromModel->getTriangleEdgeShell(k)[0];
                                    }

                                    bool is_present = false;
                                    unsigned int k0 = 0;
                                    while((!is_present) && k0 < i)
                                    {
                                        is_present = (ind_test == tab[k0]);
                                        k0+=1;
                                    }
                                    if(!is_present)
                                    {

                                        Edge t;

                                        const Triangle &te=triangleArray[ind_test];
                                        int h = fromModel->getEdgeIndexInTriangle(fromModel->getEdgeTriangleShell(ind_test),k);

                                        t[0]=(int)(te[(h+1)%3]); t[1]=(int)(te[(h+2)%3]);

                                        // sort t such that t[0] is the smallest one
                                        if ((t[0]>t[1]))
                                        {
                                            int val=t[0]; t[0]=t[1]; t[1]=val;
                                        }

                                        edges_to_create.push_back(t);
                                        edgesIndexList.push_back(nb_elems);
                                        nb_elems+=1;

                                        Loc2GlobVec.push_back(k);
                                        std::map<unsigned int, unsigned int>::iterator iter_1 = Glob2LocMap.find(k);
                                        if(iter_1 != Glob2LocMap.end() )
                                        {
                                            std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - fail to add edge " << k << "which already exists" << std::endl;
                                            Glob2LocMap.erase(Glob2LocMap.find(k));
                                        }
                                        Glob2LocMap[k]=Loc2GlobVec.size()-1;
                                    }
                                }
                            }
                        }

                        to_tstm->addEdgesProcess(edges_to_create) ;
                        to_tstm->addEdgesWarning(edges_to_create.size(), edges_to_create, edgesIndexList) ;
                        //to_tstm->propagateTopologicalChanges();

                    }

                    break;
                }

                case core::componentmodel::topology::POINTSREMOVED:
                {
                    //std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - POINTSREMOVED" << std::endl;

                    const sofa::helper::vector<unsigned int> tab = ( static_cast< const sofa::component::topology::PointsRemoved * >( *itBegin ) )->getArray();

                    sofa::helper::vector<unsigned int> indices;

                    for(unsigned int i = 0; i < tab.size(); ++i)
                    {

                        indices.push_back(tab[i]);
                    }

                    sofa::helper::vector<unsigned int>& tab_indices = indices;

                    to_tstm->removePointsWarning(tab_indices, false);
                    to_tstm->propagateTopologicalChanges();
                    to_tstm->removePointsProcess(tab_indices, false);

                    break;
                }

                case core::componentmodel::topology::POINTSRENUMBERING:
                {
                    //std::cout << "INFO_print : Hexa2TriangleTopologicalMapping - POINTSREMOVED" << std::endl;

                    const sofa::helper::vector<unsigned int> &tab = ( static_cast< const PointsRenumbering * >( *itBegin ) )->getIndexArray();
                    const sofa::helper::vector<unsigned int> &inv_tab = ( static_cast< const PointsRenumbering * >( *itBegin ) )->getinv_IndexArray();

                    sofa::helper::vector<unsigned int> indices;
                    sofa::helper::vector<unsigned int> inv_indices;

                    for(unsigned int i = 0; i < tab.size(); ++i)
                    {

                        //std::cout << "INFO_print : Hexa2TriangleTopologicalMapping - point = " << tab[i] << std::endl;
                        indices.push_back(tab[i]);
                        inv_indices.push_back(inv_tab[i]);
                    }

                    sofa::helper::vector<unsigned int>& tab_indices = indices;
                    sofa::helper::vector<unsigned int>& inv_tab_indices = inv_indices;

                    to_tstm->renumberPointsWarning(tab_indices, inv_tab_indices, false);
                    to_tstm->propagateTopologicalChanges();
                    to_tstm->renumberPointsProcess(tab_indices, inv_tab_indices, false);

                    break;
                }

                /*
                case core::componentmodel::topology::EDGESADDED:
                {

                	//std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - EDGESADDED" << std::endl;

                	if (fromModel) {

                		const sofa::component::topology::EdgesAdded *ta=static_cast< const sofa::component::topology::EdgesAdded * >( *itBegin );

                		sofa::helper::vector< Edge > edges_to_create;
                		sofa::helper::vector< unsigned int > edgesIndexList;
                		int nb_elems = toModel->getNbEdges();

                		for (unsigned int i=0;i<ta->getNbAddedEdges();++i)
                		{
                			Edge t = ta->edgeArray[i];
                			if ((t[0]>t[1])) {
                				int val=t[0]; t[0]=t[1]; t[1]=val;
                			}
                			unsigned int k = ta->edgeIndexArray[i];

                			edges_to_create.push_back(t);
                			edgesIndexList.push_back(Loc2GlobVec.size());
                			nb_elems+=1;

                			Loc2GlobVec.push_back(k);
                			std::map<unsigned int, unsigned int>::iterator iter_1 = Glob2LocMap.find(k);
                			if(iter_1 != Glob2LocMap.end() ) {
                				std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - fail to add edge " << k << "which already exists" << std::endl;
                				Glob2LocMap.erase(Glob2LocMap.find(k));
                			}
                			Glob2LocMap[k]=Loc2GlobVec.size()-1;
                		}

                		to_tstm->addEdgesProcess(edges_to_create) ;
                		to_tstm->addEdgesWarning(edges_to_create.size(), edges_to_create, edgesIndexList) ;
                		//toModel->propagateTopologicalChanges();

                	}

                	break;
                }
                */

                case core::componentmodel::topology::TRIANGLESADDED:
                {

                    //std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - TRIANGLESADDED" << std::endl;

                    if (fromModel)
                    {

                        //const sofa::helper::vector<Triangle> &triangleArray=fromModel->getTriangles();

                        const sofa::component::topology::TrianglesAdded *ta=static_cast< const sofa::component::topology::TrianglesAdded * >( *itBegin );

                        sofa::helper::vector< Edge > edges_to_create;
                        sofa::helper::vector< unsigned int > edgesIndexList;

                        for (unsigned int i=0; i<ta->getNbAddedTriangles(); ++i)
                        {
                            unsigned int ind_elem = ta->triangleIndexArray[i];
                            for (unsigned int j = 0; j < 3; ++j)
                            {
                                unsigned int k = (fromModel->getEdgeTriangleShell(ind_elem))[j];

                                std::map<unsigned int, unsigned int>::iterator iter_1 = Glob2LocMap.find(k);
                                bool is_present = (iter_1 != Glob2LocMap.end()) && fromModel->getTriangleEdgeShell(k).size()>1;

                                if (is_present)   // remove as visible the edge indexed by k
                                {

                                    unsigned int ind_k = Glob2LocMap[k];
                                    unsigned int ind_last = Loc2GlobVec.size() - 1;
                                    unsigned int ind_real_last = Loc2GlobVec[ind_last];

                                    Glob2LocMap.erase(Glob2LocMap.find(ind_real_last));
                                    Glob2LocMap[ind_real_last] = ind_k;

                                    Loc2GlobVec[ind_k] = ind_real_last;

                                    Glob2LocMap.erase(Glob2LocMap.find(k));
                                    Loc2GlobVec.resize(Loc2GlobVec.size() - 1);

                                    sofa::helper::vector< unsigned int > edges_to_remove;
                                    edges_to_remove.push_back(ind_k);
                                    to_tstm->removeEdges(edges_to_remove, false);

                                }
                                else   // add as visible the edge indexed by k
                                {

                                    if((iter_1 == Glob2LocMap.end()) && (fromModel->getTriangleEdgeShell(k).size()==1))
                                    {

                                        //sofa::helper::vector< Edge > edges_to_create;
                                        //sofa::helper::vector< unsigned int > edgesIndexList;

                                        Edge t = fromModel->getEdge(k);
                                        /*
                                        if ((t[0]>t[1])) {
                                        	int val=t[0]; t[0]=t[1]; t[1]=val;
                                        }
                                        */

                                        edges_to_create.push_back(t);
                                        edgesIndexList.push_back(Loc2GlobVec.size());

                                        Loc2GlobVec.push_back(k);
                                        std::map<unsigned int, unsigned int>::iterator iter_1 = Glob2LocMap.find(k);
                                        if(iter_1 != Glob2LocMap.end() )
                                        {
                                            std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - fail to add edge " << k << "which already exists" << std::endl;
                                            Glob2LocMap.erase(Glob2LocMap.find(k));
                                        }
                                        Glob2LocMap[k]=Loc2GlobVec.size()-1;

                                        //to_tstm->addEdgesProcess(edges_to_create) ;
                                        //to_tstm->addEdgesWarning(edges_to_create.size(), edges_to_create, edgesIndexList) ;
                                        //toModel->propagateTopologicalChanges();
                                    }

                                }
                            }
                        }

                        to_tstm->addEdgesProcess(edges_to_create) ;
                        to_tstm->addEdgesWarning(edges_to_create.size(), edges_to_create, edgesIndexList) ;
                        to_tstm->propagateTopologicalChanges();

                    }

                    break;
                }


                case core::componentmodel::topology::POINTSADDED:
                {
                    //std::cout << "INFO_print : Triangle2EdgeTopologicalMapping - POINTSADDED" << std::endl;

                    const sofa::component::topology::PointsAdded *ta=static_cast< const sofa::component::topology::PointsAdded * >( *itBegin );
                    to_tstm->addPointsProcess(ta->getNbAddedVertices());
                    to_tstm->addPointsWarning(ta->getNbAddedVertices(), ta->ancestorsList, ta->coefs, false);
                    to_tstm->propagateTopologicalChanges();

                    break;
                }


                default:
                    // Ignore events that are not Edge  related.
                    break;
                };

                ++itBegin;
            }
            to_tstm->propagateTopologicalChanges();

        }
    }

    return;
}

} // namespace topology

} // namespace component

} // namespace sofa

