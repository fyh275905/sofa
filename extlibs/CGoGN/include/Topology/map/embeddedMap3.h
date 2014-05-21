/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __EMBEDDED_MAP3_H__
#define __EMBEDDED_MAP3_H__

#include "Topology/map/map3.h"

namespace CGoGN
{

/*! Class of 3-dimensional maps with managed embeddings
 */
class EmbeddedMap3 : public Map3
{
public:
    typedef Map3 TOPO_MAP;

    static const unsigned int DIMENSION = 3 ;


    EmbeddedMap3() : Map3() {
        std::cerr << "EMAP3  creation" << std::endl;
        if(!isOrbitEmbedded<VERTEX>())
            addEmbedding<VERTEX>() ;
        if(!isOrbitEmbedded<EDGE>())
            addEmbedding<EDGE>() ;
        if(!isOrbitEmbedded<FACE>())
            addEmbedding<FACE>() ;
        if(!isOrbitEmbedded<VOLUME>())
            addEmbedding<VOLUME>() ;

    }

    std::string orbitName(unsigned int ORBIT);

    template<unsigned int ORB>
    void printEmbedding() {
        const AttributeContainer& orbCont = m_attribs[ORB] ;
        const unsigned int size = orbCont.size();
        AttributeMultiVector<unsigned int>* embVec = getEmbeddingAttributeVector(ORB);
        std::cerr << "***** printing "<< this->orbitName(ORB) << " embeddings ***** " << std::endl;
        TraversorV<EmbeddedMap3> trav(*this);
        unsigned i = 0u ;
        for (Dart d = trav.begin() ; d != trav.end() ; ++i, d = trav.next()) {
            std::cerr << "embedding number " << i << " : " << getEmbedding<ORB>(d) << std::endl;
        }
        std::cerr << "**** end embedding *****" << std::endl;
    }

    template<unsigned int ORB>
    void swapEmbeddings(unsigned int e1, unsigned int e2) {
        const AttributeContainer& dartCont = m_attribs[DART] ;
        const unsigned int dartNb = dartCont.size();

        AttributeMultiVector<unsigned int>* embVec = getEmbeddingAttributeVector(ORB);
        for (unsigned i = 0u ; i < dartNb ; ++i ) {
            unsigned int& embi = embVec->operator [](i);
            if (embi== e1)
                embi = e2;
            else {
                if (embi == e2)
                    embi = e1;
            }
        }
    }

//    template<unsigned int ORB>
//    bool copyOnFreeCell(unsigned int emb) {
//        const AttributeContainer& dartCont = m_attribs[DART] ;
//        const unsigned int dartNb = dartCont.size();

//        AttributeMultiVector<unsigned int>* embVec = getEmbeddingAttributeVector(ORB);
//        dartCont
//    }

    //!
    /*!
     *
     */
    virtual Dart splitVertex(std::vector<Dart>& vd);

    //!
    /*!
     */
    virtual Dart deleteVertex(Dart d);

    //! No attribute is attached to the new vertex
    /*! The attributes attached to the old edge are duplicated on both resulting edges
     *  @param d a dart
     */
    virtual Dart cutEdge(Dart d);

    //! The attributes attached to the edge of d are kept on the resulting edge
    /*!  @param d a dart of the edge to cut
     */
    virtual bool uncutEdge(Dart d);

    //!
    /*!
     */
    virtual Dart deleteEdge(Dart d);

    //!
    /*!
     */
    bool edgeCanCollapse(Dart d);

    //!
    /*!
     */
    virtual Dart collapseEdge(Dart d, bool delDegenerateVolumes=true);

    //!
    /*!
     */
    //	virtual bool collapseDegeneratedFace(Dart d);

    //!
    /*!
     */
    virtual void splitFace(Dart d, Dart e);

    /**
     * The attributes attached to the face of dart d are kept on the resulting face
     */
    virtual bool mergeFaces(Dart d);

    //!
    /*!
     *
     */
    virtual Dart collapseFace(Dart d, bool delDegenerateVolumes = true);

    //!
    /*!
     */
    virtual void sewVolumes(Dart d, Dart e, bool withBoundary = true);

    //!
    /*!
     */
    virtual void unsewVolumes(Dart d, bool withBoundary = true);

    //!
    /*!
     */
    virtual bool mergeVolumes(Dart d, bool deleteFace = true);


    virtual void deleteVolume(Dart d, bool withBoundary = true);

    //!
    /*!
     */
    virtual void splitVolume(std::vector<Dart>& vd);

    //!
    virtual void splitVolumeWithFace(std::vector<Dart>& vd, Dart d);


    //!
    /*!
     */
    virtual Dart collapseVolume(Dart d, bool delDegenerateVolumes = true);

    //    virtual void deleteVolume(Dart d, bool withBoundary = true) { this->deleteVolume(d, withBoundary); }
    //    virtual void deleteVolume(Dart d, bool withBoundary = true, std::vector<Dart>* removedVertices = NULL);

    //!
    /*! No attribute is attached to the new volume
     */
    virtual unsigned int closeHole(Dart d, bool forboundary = true);

    //!
    /*!
     */
    virtual bool check();
} ;

} // namespace CGoGN

#endif
