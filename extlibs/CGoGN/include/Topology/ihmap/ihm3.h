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
****************************************************************/


#ifndef __IMPLICIT_HIERARCHICAL_MAP3__
#define __IMPLICIT_HIERARCHICAL_MAP3__

#include "Topology/map/embeddedMap3.h"
#include "Algo/Multiresolution/filter.h"



namespace CGoGN
{

namespace Algo
{

namespace Volume
{

namespace IHM
{

template<typename T, unsigned int ORBIT> class AttributeHandler_IHM ;

class ImplicitHierarchicalMap3 : public EmbeddedMap3
{
    template<typename T, unsigned int ORBIT> friend class AttributeHandler_IHM ;

public:
	FunctorType* vertexVertexFunctor ;
	FunctorType* edgeVertexFunctor ;
	FunctorType* faceVertexFunctor ;
	FunctorType* volumeVertexFunctor ;

    unsigned int m_curLevel ;
    unsigned int m_maxLevel ;
    unsigned int m_edgeIdCount ;
    unsigned int m_faceIdCount;

	DartAttribute<unsigned int, EmbeddedMap3> m_dartLevel ;
	DartAttribute<unsigned int, EmbeddedMap3> m_edgeId ;
	DartAttribute<unsigned int, EmbeddedMap3> m_faceId ;

    AttributeMultiVector<unsigned int>* m_nextLevelCell[NB_ORBITS] ;

//    std::vector<Algo::MR::Filter*> synthesisFilters ;
//    std::vector<Algo::MR::Filter*> analysisFilters ;

public:
    ImplicitHierarchicalMap3() ;

    ~ImplicitHierarchicalMap3() ;

    static const unsigned int DIMENSION = 3 ;

    //!
    /*!
     *
     */
    void update_topo_shortcuts();

    //!
    /*!
     *
     */
    void initImplicitProperties() ;

    /**
     * clear the map
     * @param remove attrib remove attribute (not only clear the content)
     */
    void clear(bool removeAttrib);

    /*! @name Attributes Management
     *  To handles Attributes for each level of an implicit 3-map
     *************************************************************************/

    //@{
    //!
    /*!
     *
     */
//    template <typename T, unsigned int ORBIT>
//    AttributeHandler_IHM<T, ORBIT> addAttribute(const std::string& nameAttr) ;

    //!
    /*!
     *
     */
//    template <typename T, unsigned int ORBIT>
//    AttributeHandler_IHM<T, ORBIT> getAttribute(const std::string& nameAttr) ;
    //@}

    /*! @name Basic Topological Operators
     *  Redefinition of the basic topological operators
     *************************************************************************/

    //@{
    virtual Dart newDart() ;

    Dart phi1(Dart d) const;

    Dart phi_1(Dart d) const;

    Dart phi2(Dart d) const;

private:
    Dart phi2bis(Dart d) const;

public:
    Dart phi3(Dart d) const;

    Dart alpha0(Dart d) const;

    Dart alpha1(Dart d) const;

    Dart alpha2(Dart d) const;

    Dart alpha_2(Dart d) const;
    //@}

	/*! @name Topological Operators with Cells id management
	 *  Topological operations on Hierarchical Implicit 3-maps
	 *************************************************************************/

//	void deleteVolume(Dart d);

	bool isWellEmbedded();

	//@{

	//!
	/*!
	 *
	 */
	void swapEdges(Dart d, Dart e);





//    void addSynthesisFilter(Algo::MR::Filter* f) { synthesisFilters.push_back(f) ; }
//    void addAnalysisFilter(Algo::MR::Filter* f) { analysisFilters.push_back(f) ; }

//    void clearSynthesisFilters() { synthesisFilters.clear() ; }
//    void clearAnalysisFilters() { analysisFilters.clear() ; }

//    void analysis() ;
//    void synthesis() ;

	//!
	/*!
	 *
	 */
	void saveRelationsAroundVertex(Dart d, std::vector<std::pair<Dart, Dart> >& vd);

	void unsewAroundVertex(std::vector<std::pair<Dart, Dart> >& vd);

	Dart quadranguleFace(Dart d);

	void deleteVertexSubdividedFace(Dart d);
	//@}

	void setVertexVertexFunctor(FunctorType* f) { vertexVertexFunctor = f ; }
	void setEdgeVertexFunctor(FunctorType* f) { edgeVertexFunctor = f ; }
	void setFaceVertexFunctor(FunctorType* f) { faceVertexFunctor = f ; }
	void setVolumeVertexFunctor(FunctorType* f) { volumeVertexFunctor = f ; }

	void computeVertexVertexFunctor(Dart d) { (*vertexVertexFunctor)(d); }
	void computeEdgeVertexFunctor(Dart d) { (*edgeVertexFunctor)(d); }
	void computeFaceVertexFunctor(Dart d) { (*faceVertexFunctor)(d); }
	void computerVolumeVertexFunctor(Dart d) { (*volumeVertexFunctor)(d); }

	/*! @name Levels Management
	 *  Operations to manage the levels of an Implicit Hierarchical 3-map
	 *************************************************************************/

    void incCurrentLevel();

    void decCurrentLevel();


    //@{
    //!
    /*!
     *
     */
    unsigned int getCurrentLevel() const ;

    //!
    /*!
     *
     */
    void setCurrentLevel(unsigned int l) ;

    //!
    /*!
     *
     */
    unsigned int getMaxLevel() const ;

    //!
    /*!
     *
     */
    unsigned int getDartLevel(Dart d) const ;

    //!
    /*!
     *
     */
    void setDartLevel(Dart d, unsigned int i) ;
	//@}

	/*! @name Id Management
	 * Operations to manage the ids of edges and faces
	 *************************************************************************/

	//@{
    //! Give a new unique id to all the edges of the map
    /*!
     */
    void initEdgeId() ;

    //! Return the next available edge id
    /*!
     */
    unsigned int getNewEdgeId() ;

    //! Return the id of the edge of d
    /*!
     */
    unsigned int getEdgeId(Dart d) ;

    //! Set an edge id to all darts from an orbit of d
    /*!
     */
    void setEdgeId(Dart d, unsigned int i, unsigned int orbit); //TODO a virer
    void setEdgeId(Dart d, unsigned int i);

    //! Give a new unique id to all the faces of the map
    /*!
     */
    void initFaceId() ;

    //! Return the next available face id
    /*!
     */
    unsigned int getNewFaceId() ;

    //! Return the id of the face of d
    /*!
     */
    unsigned int getFaceId(Dart d) ;

    //! Set a face id to all darts from an orbit of d
    /*!
     */
    void setFaceId(Dart d, unsigned int i, unsigned int orbit); //TODO a virer
    void setFaceId(unsigned int orbit, Dart d);
	//@}

	/*! @name Cells Information
	 * Operations to manage the cells informations :
	 *************************************************************************/

	//@{
    //! Return the level of insertion of the vertex of d
    /*!
     */
    unsigned int vertexInsertionLevel(Dart d) const;

	//! Return the level of the edge of d in the current level map
	/*!
	 */
	unsigned int edgeLevel(Dart d) ;

	//! Return the level of the face of d in the current level map
	/*!
	 */
	unsigned int faceLevel(Dart d);

	//! Return the level of the volume of d in the current level map
	/*!
	 */
    unsigned int volumeLevel(Dart d);

	//! Return the oldest dart of the face of d in the current level map
	/*!
	 */
	Dart faceOldestDart(Dart d);

	//! Return the oldest dart of the volume of d in the current level map
	/*!
	 */
	Dart volumeOldestDart(Dart d);

	//! Return true if the edge of d in the current level map
	//! has already been subdivided to the next level
	/*!
	 */
	bool edgeIsSubdivided(Dart d) ;

	//! Return true if the edge of d in the current level map
	//! is subdivided to the next level,
	//! none of its resulting edges is in turn subdivided to the next level
	//! and the middle vertex is of degree 2
	/*!
	 */
	bool edgeCanBeCoarsened(Dart d);

	//! Return true if the face of d in the current level map
	//! has already been subdivided to the next level
	/*!
	 */
	bool faceIsSubdivided(Dart d) ;

	//!
	/*!
	 */
	bool faceCanBeCoarsened(Dart d);

	//! Return true if the volume of d in the current level map
	//! has already been subdivided to the next level
	/*!
	 */
	bool volumeIsSubdivided(Dart d);

	//!
	/*!
	 */
	bool volumeIsSubdividedOnce(Dart d);



	/*! @name
	 *************************************************************************/

	//!
	/*!
	 */
	bool neighborhoodLevelDiffersMoreThanOne(Dart d);

	//! wired !!!
	/*!
	 */
	bool coarsenNeighborhoodLevelDiffersMoreThanOne(Dart d);
	//@}

	/*! @name Cell Functors
	 * Redefition of the 3-maps map traversor
	 *************************************************************************/

	//@{
    Dart begin() const;

    Dart end() const;

    void next(Dart& d) const ;

	template <unsigned int ORBIT, typename FUNC>
	void foreach_dart_of_orbit(Cell<ORBIT> c, FUNC f) const ;
//	template <unsigned int ORBIT, typename FUNC>
//	void foreach_dart_of_orbit(Cell<ORBIT> c, FUNC& f) const ;

	template <typename FUNC>
    void foreach_dart_of_vertex(Dart d, const FUNC& f) const ;

	template <typename FUNC>
    void foreach_dart_of_edge(Dart d, const FUNC& f) const ;

	template <typename FUNC>
    void foreach_dart_of_oriented_face(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_face(Dart d, const FUNC& f) const ;

	template <typename FUNC>
    void foreach_dart_of_oriented_volume(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_volume(Dart d, const FUNC& f) const ;

	template <typename FUNC>
    void foreach_dart_of_vertex1(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_edge1(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_vertex2(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_edge2(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_face2(Dart d, const FUNC& f) const;

	template <typename FUNC>
    void foreach_dart_of_cc(Dart d, const FUNC& f) const ;
	//@}

//    template <unsigned int ORBIT>
//	unsigned int getEmbedding(Cell<ORBIT> c) const;
} ;
} // namespace IHM
} // namespace Volume
} // namespace Algo

template <typename T, unsigned int ORBIT>
class AttributeHandler< T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3 > : public AttributeHandlerGen
{
public:
    typedef Algo::Volume::IHM::ImplicitHierarchicalMap3 MAP;
    typedef T DATA_TYPE ;
protected:
    MAP* m_map;
    AttributeMultiVector<T>* m_attrib;

    void registerInMap() ;
    void unregisterFromMap() ;
private:
    template <unsigned int ORBIT2>
    AttributeHandler(const AttributeHandler<T, ORBIT2, MAP>& h) ;
    template <unsigned int ORBIT2>
    AttributeHandler<T, ORBIT, MAP>& operator=(const AttributeHandler<T, ORBIT2, MAP>& ta) ;
public:
    AttributeHandler< T, ORBIT, MAP >() ;
    AttributeHandler< T, ORBIT, MAP >(MAP* m, AttributeMultiVector<T>* amv) ;
    AttributeHandler< T, ORBIT, MAP >(const AttributeHandler<T, ORBIT, MAP>& ta) ;
    AttributeHandler<T, ORBIT, MAP>& operator=(const AttributeHandler<T, ORBIT, MAP>& ta) ;
    inline 	MAP* map() const
    {
        return m_map ;
    }
    AttributeMultiVector<T>* getDataVector() const ;
    virtual AttributeMultiVectorGen* getDataVectorGen() const ;
    virtual int getSizeOfType() const ;
    virtual unsigned int getOrbit() const ;
    unsigned int getIndex() const ;
    virtual const std::string& name() const ;
    virtual const std::string& typeName() const ;
    unsigned int nbElements() const;
    T& operator[](Dart d) ;
    const T& operator[](Dart d) const ;
    inline T& operator[](Cell<ORBIT> c)
    {
        return this->operator [](c.dart);
    }
    inline const T& operator[](Cell<ORBIT> c) const
    {
        return this->operator [](c.dart);
    }
    T& operator[](unsigned int a) ;
    const T& operator[](unsigned int a) const ;
    unsigned int insert(const T& elt) ;
    unsigned int newElt() ;
    void setAllValues(const T& v) ;
    unsigned int begin() const;
    unsigned int end() const;
    void next(unsigned int& iter) const;
    virtual ~AttributeHandler() ;
};
template <typename T, unsigned int ORBIT>
CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::AttributeHandler(const AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3> &ta) :
    AttributeHandlerGen(ta.valid),
    m_map(ta.m_map),
    m_attrib(ta.m_attrib)
{
    if(valid)
        registerInMap() ;
}

template <typename T, unsigned int ORBIT>
CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::~AttributeHandler()
{
    if(valid)
        unregisterFromMap() ;
}

template <typename T, unsigned int ORBIT>
void CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::next(unsigned int &iter) const
{
    assert(valid || !"Invalid AttributeHandler") ;
    m_map->template getAttributeContainer<ORBIT>().next(iter) ;
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::end() const
{
    assert(valid || !"Invalid AttributeHandler") ;
    return m_map->template getAttributeContainer<ORBIT>().end() ;
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::begin() const
{
    assert(valid || !"Invalid AttributeHandler") ;
    return m_map->template getAttributeContainer<ORBIT>().begin() ;
}

template <typename T, unsigned int ORBIT>
void CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::setAllValues(const T &v)
{
    for(unsigned int i = begin(); i != end(); next(i))
        m_attrib->operator[](i) = v ;
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::newElt()
{
    assert(valid || !"Invalid AttributeHandler") ;
    unsigned int idx = m_map->template getAttributeContainer<ORBIT>().insertLine() ;
    return idx ;
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::insert(const T &elt)
{
    assert(valid || !"Invalid AttributeHandler") ;
    unsigned int idx = m_map->template getAttributeContainer<ORBIT>().insertLine() ;
    m_attrib->operator[](idx) = elt ;
    return idx ;
}

template <typename T, unsigned int ORBIT>
const T &CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::operator[](unsigned int a) const
{
    assert(valid || !"Invalid AttributeHandler") ;
    return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORBIT>
T& CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::operator[](unsigned int a)
{
    assert(valid || !"Invalid AttributeHandler") ;
    return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORBIT>
T& CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::operator[](Dart d)
{
    MAP* m = this->m_map;
    assert(m->m_dartLevel[d] <= m->m_curLevel || !"Access to a dart introduced after current level") ;
    assert(m->vertexInsertionLevel(d) <= m->m_curLevel || !"Access to the embedding of a vertex inserted after current level") ;

//	std::cout << std::endl << "vertexInsertionLevel[" << d <<"] = " << m->vertexInsertionLevel(d) << "\t";

    unsigned int nbSteps = m->m_curLevel - m->vertexInsertionLevel(d) ;
    unsigned int index = m->EmbeddedMap3::getEmbedding<ORBIT>(d) ;

//	std::cout << " m->vertexInsertionLevel(d) = " <<  m->vertexInsertionLevel(d) << std::endl;
//	std::cout << "m_curLevel = " << m->m_curLevel << std::endl;
//	std::cout << " nbSteps = " <<  nbSteps << std::endl;
//	std::cout << "index EmbMap3 = " << index << std::endl;

    if(index == EMBNULL)
    {
        index = Algo::Topo::setOrbitEmbeddingOnNewCell<ORBIT>(*m, d) ;
        m->m_nextLevelCell[ORBIT]->operator[](index) = EMBNULL ;
    }

    AttributeContainer& cont = m->getAttributeContainer<ORBIT>() ;
    unsigned int step = 0 ;
    while(step < nbSteps)
    {
        step++ ;
        unsigned int nextIdx = m->m_nextLevelCell[ORBIT]->operator[](index) ;
        if (nextIdx == EMBNULL)
        {
            nextIdx = m->newCell<ORBIT>() ;
            m->copyCell<ORBIT>(nextIdx, index) ;
            m->m_nextLevelCell[ORBIT]->operator[](index) = nextIdx ;
            m->m_nextLevelCell[ORBIT]->operator[](nextIdx) = EMBNULL ;
            cont.refLine(index) ;
        }
        index = nextIdx ;
    }

//	std::cout << "emb = " << index << std::endl;

//	std::cout << "index IHM = " << index << std::endl;
//	if(index != EMBNULL)
//		std::cout << " emb = " << this->m_attrib->operator[](index) << std::endl << std::endl;

    return this->m_attrib->operator[](index);
}

template <typename T, unsigned int ORBIT>
const T& CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::operator[](Dart d) const
{
    MAP* m = this->m_map ;
    assert(m->m_dartLevel[d] <= m->m_curLevel || !"Access to a dart introduced after current level") ;
    assert(m->vertexInsertionLevel(d) <= m->m_curLevel || !"Access to the embedding of a vertex inserted after current level") ;

    unsigned int nbSteps = m->m_curLevel - m->vertexInsertionLevel(d) ;
    unsigned int index = m->EmbeddedMap3::getEmbedding<ORBIT>(d) ;

//	std::cout << "(const) m->vertexInsertionLevel(d) = " <<  m->vertexInsertionLevel(d) << std::endl;
//	std::cout << "(const) m_curLevel = " << m->m_curLevel << std::endl;
//	std::cout << "(const) nbSteps = " <<  nbSteps << std::endl;
//	std::cout << "(const) index EmbMap3 = " << index << std::endl;

    unsigned int step = 0 ;
    while(step < nbSteps)
    {
        step++ ;
        unsigned int nextIdx = m->m_nextLevelCell[ORBIT]->operator[](index) ;
        if(nextIdx != EMBNULL) index = nextIdx ;
        else break ;
    }
//	if(index != EMBNULL)
//		std::cout << "(const) emb = " << this->m_attrib->operator[](index) << std::endl << std::endl;
    return this->m_attrib->operator[](index);
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::nbElements() const
{
    return m_map->template getAttributeContainer<ORBIT>().size() ;
}

template <typename T, unsigned int ORBIT>
const std::string &CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::typeName() const
{
    return m_attrib->getTypeName();
}

template <typename T, unsigned int ORBIT>
const std::string &CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::name() const
{
    return m_attrib->getName() ;
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::getIndex() const
{
    return m_attrib->getIndex() ;
}

template <typename T, unsigned int ORBIT>
unsigned int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::getOrbit() const
{
    return ORBIT ;
}

template <typename T, unsigned int ORBIT>
int CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::getSizeOfType() const
{
    return sizeof(T) ;
}

template <typename T, unsigned int ORBIT>
AttributeMultiVectorGen *CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::getDataVectorGen() const
{
    return m_attrib ;
}

template <typename T, unsigned int ORBIT>
AttributeMultiVector<T> *CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::getDataVector() const
{
    return m_attrib ;
}

template <typename T, unsigned int ORBIT>
AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3> & CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::operator=(const AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3> &ta)
{
    if(valid)
        unregisterFromMap() ;
    m_map = ta.m_map ;
    m_attrib = ta.m_attrib ;
    valid = ta.valid ;
    if(valid)
        registerInMap() ;
    return *this ;
}

template <typename T, unsigned int ORBIT>
CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::AttributeHandler(CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::MAP *m, AttributeMultiVector<T> *amv) :
    AttributeHandlerGen(false),
    m_map(m),
    m_attrib(amv)
{
    if(m != NULL && amv != NULL && amv->getIndex() != AttributeContainer::UNKNOWN)
    {
        assert(ORBIT == amv->getOrbit() || !"AttributeHandler: orbit incompatibility") ;
        valid = true ;
        registerInMap() ;
    }
    else
        valid = false ;
}

template <typename T, unsigned int ORBIT>
CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::AttributeHandler():
    AttributeHandlerGen(false),
    m_map(NULL),
    m_attrib(NULL)
{

}

template <typename T, unsigned int ORBIT>
void CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::unregisterFromMap()
{
    typedef std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator IT ;

    boost::mutex::scoped_lock lockAH(m_map->attributeHandlersMutex);
    std::pair<IT, IT> bounds = m_map->attributeHandlers.equal_range(m_attrib) ;
    for(IT i = bounds.first; i != bounds.second; ++i)
    {
        if((*i).second == this)
        {
            m_map->attributeHandlers.erase(i) ;
            return ;
        }
    }
    assert(false || !"Should not get here") ;
}

template <typename T, unsigned int ORBIT>
void CGoGN::AttributeHandler<T, ORBIT, Algo::Volume::IHM::ImplicitHierarchicalMap3>::registerInMap()
{
    boost::mutex::scoped_lock lockAH(m_map->attributeHandlersMutex);
    m_map->attributeHandlers.insert(std::pair<AttributeMultiVectorGen*, AttributeHandlerGen*>(m_attrib, this)) ;
}


//template <typename T, unsigned int ORBIT>
//class AttributeHandler_IHM : public AttributeHandler<T, ORBIT, ImplicitHierarchicalMap3>
//{
//public:
//    typedef T DATA_TYPE ;

//    AttributeHandler_IHM() : AttributeHandler<T, ORBIT, ImplicitHierarchicalMap3>()
//    {}

//    AttributeHandler_IHM(ImplicitHierarchicalMap3* m, AttributeMultiVector<T>* amv) : AttributeHandler<T, ORBIT, ImplicitHierarchicalMap3>(m, amv)
//    {}



//    virtual T& operator[](Dart d) ;
//    virtual const T& operator[](Dart d) const ;

//    T& operator[](unsigned int a)
//    {
//        return AttributeHandler<T, ORBIT, ImplicitHierarchicalMap3>::operator[](a) ;
//    }

//    const T& operator[](unsigned int a) const
//    {
//        return AttributeHandler<T, ORBIT, ImplicitHierarchicalMap3>::operator[](a) ;
//    }

//} ;

//template <typename T>
//class VertexAttribute_IHM : public IHM::AttributeHandler_IHM<T, VERTEX>
//{
//public:
//    VertexAttribute_IHM() : IHM::AttributeHandler_IHM<T, VERTEX>() {}
//    VertexAttribute_IHM(const IHM::AttributeHandler_IHM<T, VERTEX>& ah) : IHM::AttributeHandler_IHM<T, VERTEX>(ah) {}
////	VertexAttribute_IHM<T>& operator=(const IHM::AttributeHandler_IHM<T, VERTEX>& ah) { this->IHM::AttributeHandler_IHM<T, VERTEX>::operator=(ah); return *this; }
//};

//} // namespace IHM
//} // namespace Volume
//} // namespace Algo

} // namespace CGoGN

#include "Topology/ihmap/ihm3.hpp"

#endif
