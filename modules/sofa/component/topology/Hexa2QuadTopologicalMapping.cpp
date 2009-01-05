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
#include <sofa/component/topology/Hexa2QuadTopologicalMapping.h>

#include <sofa/core/ObjectFactory.h>

#include <sofa/component/topology/QuadSetTopologyContainer.h>
#include <sofa/component/topology/QuadSetTopologyModifier.h>
#include <sofa/component/topology/QuadSetTopologyChange.h>

#include <sofa/component/topology/HexahedronSetTopologyContainer.h>
#include <sofa/component/topology/HexahedronSetTopologyModifier.h>
#include <sofa/component/topology/HexahedronSetTopologyChange.h>

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

SOFA_DECL_CLASS(Hexa2QuadTopologicalMapping)

// Register in the Factory
int Hexa2QuadTopologicalMappingClass = core::RegisterObject("Special case of mapping where HexahedronSetTopology is converted to QuadSetTopology")
        .add< Hexa2QuadTopologicalMapping >()

        ;

// Implementation

Hexa2QuadTopologicalMapping::Hexa2QuadTopologicalMapping(In* from, Out* to)
    : TopologicalMapping(from, to),
      object1(initData(&object1, std::string("../.."), "object1", "First object to map")),
      object2(initData(&object2, std::string(".."), "object2", "Second object to map"))
{
}


Hexa2QuadTopologicalMapping::~Hexa2QuadTopologicalMapping()
{
}

void Hexa2QuadTopologicalMapping::init()
{
    //sout << "INFO_print : init Hexa2QuadTopologicalMapping" << sendl;

    // INITIALISATION of QUADULAR mesh from HEXAHEDRAL mesh :

    if (fromModel)
    {

        sout << "INFO_print : Hexa2QuadTopologicalMapping - from = hexa" << sendl;

        if (toModel)
        {

            sout << "INFO_print : Hexa2QuadTopologicalMapping - to = quad" << sendl;

            QuadSetTopologyContainer *to_tstc;
            toModel->getContext()->get(to_tstc);
            to_tstc->clear();

            toModel->setNbPoints(fromModel->getNbPoints());

            QuadSetTopologyModifier *to_tstm;
            toModel->getContext()->get(to_tstm);

            const sofa::helper::vector<Quad> &quadArray=fromModel->getQuads();

            unsigned int nb_visible_quads = 0;

            Loc2GlobVec.clear();
            Glob2LocMap.clear();

            for (unsigned int i=0; i<quadArray.size(); ++i)
            {

                if (fromModel->getHexaQuadShell(i).size()==1)
                {

                    to_tstm->addQuadProcess(quadArray[i]);

                    Loc2GlobVec.push_back(i);
                    Glob2LocMap[i]=Loc2GlobVec.size()-1;

                    nb_visible_quads+=1;
                }
            }

            //to_tstm->propagateTopologicalChanges();
            to_tstm->notifyEndingEvent();
            //to_tstm->propagateTopologicalChanges();
        }

    }
}

unsigned int Hexa2QuadTopologicalMapping::getFromIndex(unsigned int ind)
{

    if(fromModel->getHexaQuadShell(ind).size()==1)
    {
        return fromModel->getHexaQuadShell(ind)[0];
    }
    else
    {
        return 0;
    }
}

void Hexa2QuadTopologicalMapping::updateTopologicalMappingTopDown()
{

    // INITIALISATION of QUADULAR mesh from HEXAHEDRAL mesh :

    if (fromModel)
    {

        QuadSetTopologyModifier *to_tstm;
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
                    //sout << "INFO_print : Hexa2QuadTopologicalMapping - ENDING_EVENT" << sendl;
                    to_tstm->propagateTopologicalChanges();
                    to_tstm->notifyEndingEvent();
                    to_tstm->propagateTopologicalChanges();
                    break;
                }

                case core::componentmodel::topology::QUADSREMOVED:
                {
                    //sout << "INFO_print : Hexa2QuadTopologicalMapping - QUADSREMOVED" << sendl;

                    int last;
                    int ind_last;

                    last= fromModel->getNbQuads() - 1;

                    const sofa::helper::vector<unsigned int> &tab = ( static_cast< const QuadsRemoved *>( *itBegin ) )->getArray();

                    unsigned int ind_tmp;

                    unsigned int ind_real_last;
                    ind_last=toModel->getNbQuads();

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

                            sofa::helper::vector< unsigned int > quads_to_remove;
                            quads_to_remove.push_back(ind_k);

                            to_tstm->removeQuads(quads_to_remove, true, false);

                        }
                        else
                        {

                            sout << "INFO_print : Hexa2QuadTopologicalMapping - Glob2LocMap should have the visible quad " << tab[i] << sendl;
                            sout << "INFO_print : Hexa2QuadTopologicalMapping - nb quads = " << ind_last << sendl;
                        }

                        --last;
                    }
                    break;
                }

                case core::componentmodel::topology::HEXAHEDRAREMOVED:
                {
                    //sout << "INFO_print : Hexa2QuadTopologicalMapping - HEXAHEDRAREMOVED" << sendl;

                    if (fromModel)
                    {

                        const sofa::helper::vector<Hexahedron> &hexahedronArray=fromModel->getHexas();

                        const sofa::helper::vector<unsigned int> &tab = ( static_cast< const HexahedraRemoved *>( *itBegin ) )->getArray();

                        sofa::helper::vector< Quad > quads_to_create;
                        sofa::helper::vector< unsigned int > quadsIndexList;
                        int nb_elems = toModel->getNbQuads();

                        for (unsigned int i = 0; i < tab.size(); ++i)
                        {

                            for (unsigned int j = 0; j < 6; ++j)
                            {
                                unsigned int k = (fromModel->getQuadHexaShell(tab[i]))[j];

                                if (fromModel->getHexaQuadShell(k).size()==1)   // remove as visible the quad indexed by k
                                {

                                    // do nothing

                                }
                                else   // fromModel->getHexaQuadShell(k).size()==2 // add as visible the quad indexed by k
                                {

                                    unsigned int ind_test;
                                    if(tab[i] == fromModel->getHexaQuadShell(k)[0])
                                    {

                                        ind_test = fromModel->getHexaQuadShell(k)[1];

                                    }
                                    else   // tab[i] == fromModel->getHexaQuadShell(k)[1]
                                    {

                                        ind_test = fromModel->getHexaQuadShell(k)[0];
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

                                        Quad t;

                                        const Hexahedron &te=hexahedronArray[ind_test];
                                        int h = fromModel->getQuadIndexInHexahedron(fromModel->getQuadHexaShell(ind_test),k);
                                        //unsigned int hh = (fromModel->getQuadHexaShell(ind_test))[h];

                                        //t=from_qstc->getQuad(hh);

                                        switch( h )
                                        {

                                        case 0:
                                        {
                                            t[0] = te[0];
                                            t[1] = te[3];
                                            t[2] = te[2];
                                            t[3] = te[1];

                                            break;
                                        }
                                        case 1:
                                        {
                                            t[0] = te[4];
                                            t[1] = te[5];
                                            t[2] = te[6];
                                            t[3] = te[7];

                                            break;
                                        }
                                        case 2:
                                        {
                                            t[0] = te[0];
                                            t[1] = te[1];
                                            t[2] = te[5];
                                            t[3] = te[4];

                                            break;
                                        }
                                        case 3:
                                        {
                                            t[0] = te[1];
                                            t[1] = te[2];
                                            t[2] = te[6];
                                            t[3] = te[5];

                                            break;
                                        }
                                        case 4:
                                        {
                                            t[0] = te[2];
                                            t[1] = te[3];
                                            t[2] = te[7];
                                            t[3] = te[6];

                                            break;
                                        }
                                        case 5:
                                        {
                                            t[0] = te[3];
                                            t[1] = te[0];
                                            t[2] = te[4];
                                            t[3] = te[7];

                                            break;
                                        }

                                        default:
                                            break;
                                        };

                                        quads_to_create.push_back(t);
                                        quadsIndexList.push_back(nb_elems);
                                        nb_elems+=1;

                                        Loc2GlobVec.push_back(k);
                                        std::map<unsigned int, unsigned int>::iterator iter_1 = Glob2LocMap.find(k);
                                        if(iter_1 != Glob2LocMap.end() )
                                        {
                                            sout << "INFO_print : Hexa2QuadTopologicalMapping - fail to add quad " << k << "which already exists" << sendl;
                                            Glob2LocMap.erase(Glob2LocMap.find(k));
                                        }
                                        Glob2LocMap[k]=Loc2GlobVec.size()-1;
                                    }
                                }
                            }
                        }

                        to_tstm->addQuadsProcess(quads_to_create) ;
                        to_tstm->addQuadsWarning(quads_to_create.size(), quads_to_create, quadsIndexList) ;

                    }

                    break;
                }

                case core::componentmodel::topology::POINTSREMOVED:
                {
                    //sout << "INFO_print : Hexa2QuadTopologicalMapping - POINTSREMOVED" << sendl;

                    const sofa::helper::vector<unsigned int> tab = ( static_cast< const sofa::component::topology::PointsRemoved * >( *itBegin ) )->getArray();

                    sofa::helper::vector<unsigned int> indices;

                    for(unsigned int i = 0; i < tab.size(); ++i)
                    {

                        //sout << "INFO_print : Hexa2QuadTopologicalMapping - point = " << tab[i] << sendl;
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
                    //sout << "INFO_print : Hexa2QuadTopologicalMapping - POINTSREMOVED" << sendl;

                    const sofa::helper::vector<unsigned int> &tab = ( static_cast< const PointsRenumbering * >( *itBegin ) )->getIndexArray();
                    const sofa::helper::vector<unsigned int> &inv_tab = ( static_cast< const PointsRenumbering * >( *itBegin ) )->getinv_IndexArray();

                    sofa::helper::vector<unsigned int> indices;
                    sofa::helper::vector<unsigned int> inv_indices;

                    for(unsigned int i = 0; i < tab.size(); ++i)
                    {

                        //sout << "INFO_print : Hexa2QuadTopologicalMapping - point = " << tab[i] << sendl;
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

                default:
                    // Ignore events that are not Quad  related.
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

