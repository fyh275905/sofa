/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <sofa/core/fwd.h>
#include <sofa/core/VecId.h>
#include <memory>
#include <map>
#include <set>
#include "MultiVecId.h"

namespace sofa::core
{

template <VecAccess vaccess>
class SOFA_CORE_API TMultiVecId<V_ALL, vaccess>
{
public:
    typedef TVecId<V_ALL, vaccess> MyVecId;

    typedef std::map<const BaseState*, MyVecId> IdMap;
    typedef typename IdMap::iterator IdMap_iterator;
    typedef typename IdMap::const_iterator IdMap_const_iterator;

protected:
    MyVecId defaultId;

private:
    std::shared_ptr< IdMap > idMap_ptr;

	template <VecType vtype2, VecAccess vaccess2> friend class TMultiVecId;

protected:
    IdMap& writeIdMap()
    {
        if (!idMap_ptr)
            idMap_ptr.reset(new IdMap());
        else if(!idMap_ptr.unique())
            idMap_ptr.reset(new IdMap(*idMap_ptr));
        return *idMap_ptr;
    }
public:
    bool hasIdMap() const { return idMap_ptr != nullptr; }
    const  IdMap& getIdMap() const
    {
        if (!idMap_ptr)
        {
            static const IdMap empty;
            return empty;
        }
        return *idMap_ptr;
    }

public:

    TMultiVecId()
    {
    }

    /// Copy from another VecId, possibly with another type of access, with the
    /// constraint that the access must be compatible (i.e. cannot create
    /// a write-access VecId from a read-only VecId.
    template<VecType vtype2, VecAccess vaccess2>
    TMultiVecId(const TVecId<vtype2, vaccess2>& v) : defaultId(v)
    {
        static_assert(vaccess2 >= vaccess, "Copy from a read-only multi-vector id into a read/write multi-vector id is forbidden.");
    }

    /// Copy assignment from another VecId
    template<VecType vtype2, VecAccess vaccess2>
    TMultiVecId<V_ALL, vaccess> & operator= (const TVecId<vtype2, vaccess2>& v) {
        static_assert(vaccess2 >= vaccess, "Copy from a read-only multi-vector id into a read/write multi-vector id is forbidden.");
        defaultId = v;
        return *this;
    }

    //// Copy constructor
    TMultiVecId( const TMultiVecId<V_ALL,vaccess>& mv)
        : defaultId( mv.getDefaultId() )
        , idMap_ptr( mv.idMap_ptr )
    {
    }

    /// Copy assignment
    TMultiVecId<V_ALL, vaccess> & operator= (const TMultiVecId<V_ALL, vaccess>& mv) {
        defaultId = mv.getDefaultId();
        idMap_ptr = mv.idMap_ptr;
        return *this;
    }

    //// Only TMultiVecId< V_ALL , vaccess> can declare copy constructors with all
    //// other kinds of TMultiVecIds, namely MultiVecCoordId, MultiVecDerivId...
    //// In other cases, the copy constructor takes a TMultiVecId of the same type
    //// ie copy construct a MultiVecCoordId from a const MultiVecCoordId& or a
    //// ConstMultiVecCoordId&.
    template< VecType vtype2, VecAccess vaccess2>
    TMultiVecId( const TMultiVecId<vtype2,vaccess2>& mv) : defaultId( mv.getDefaultId() )
    {
        static_assert( vaccess2 >= vaccess, "Copy from a read-only multi-vector id into a read/write multi-vector id is forbidden." );

        if (mv.hasIdMap())
        {
			// When we assign a V_WRITE version to a V_READ version of the same type, which are binary compatible,
			// share the maps like with a copy constructor, because otherwise a simple operation like passing a
			// MultiVecCoordId to a method taking a ConstMultiVecCoordId to indicate it won't modify it
			// will cause a temporary copy of the map, which this define was meant to avoid!

            // Type-punning
            union {
                const std::shared_ptr< IdMap > * this_map_type;
                const std::shared_ptr< typename TMultiVecId<vtype2,vaccess2>::IdMap > * other_map_type;
            } ptr;
            ptr.other_map_type = &mv.idMap_ptr;
            idMap_ptr = *(ptr.this_map_type);
        }
    }

    template<VecType vtype2, VecAccess vaccess2>
    TMultiVecId<V_ALL, vaccess> & operator= (const TMultiVecId<vtype2, vaccess2>& mv) {
        static_assert( vaccess2 >= vaccess, "Copy from a read-only multi-vector id into a read/write multi-vector id is forbidden." );

        defaultId = mv.defaultId;
        if (mv.hasIdMap()) {
            // When we assign a V_WRITE version to a V_READ version of the same type, which are binary compatible,
            // share the maps like with a copy constructor, because otherwise a simple operation like passing a
            // MultiVecCoordId to a method taking a ConstMultiVecCoordId to indicate it won't modify it
            // will cause a temporary copy of the map, which this define was meant to avoid!

            // Type-punning
            union {
                const std::shared_ptr< IdMap > * this_map_type;
                const std::shared_ptr< typename TMultiVecId<vtype2,vaccess2>::IdMap > * other_map_type;
            } ptr;
            ptr.other_map_type = &mv.idMap_ptr;
            idMap_ptr = *(ptr.this_map_type);
        }

        return *this;
    }

    void setDefaultId(const MyVecId& id)
    {
        defaultId = id;
    }

    template<class State>
    void setId(const std::set<State>& states, const MyVecId& id)
    {
        IdMap& map = writeIdMap();
        for (const auto& it : states)
            map[*it] = id;
    }

    void setId(const BaseState* s, const MyVecId& id)
    {
        IdMap& map = writeIdMap();
        map[s] = id;
    }

    void assign(const MyVecId& id)
    {
        defaultId = id;
        idMap_ptr.reset();
    }

    const MyVecId& getId(const BaseState* s) const
    {
        if (!hasIdMap()) return defaultId;
        const IdMap& map = getIdMap();

        IdMap_const_iterator it = map.find(s);
        if (it != map.end()) return it->second;
        else                 return defaultId;
    }

    const MyVecId& getDefaultId() const
    {
        return defaultId;
    }

    std::string getName() const;

    friend inline std::ostream& operator << ( std::ostream& out, const TMultiVecId<V_ALL, vaccess>& v )
    {
        out << v.getName();
        return out;
    }

    static TMultiVecId<V_ALL, vaccess> null() { return TMultiVecId(MyVecId::null()); }
    bool isNull() const
    {
        if (!this->defaultId.isNull()) return false;
        if (hasIdMap())
            for (IdMap_const_iterator it = getIdMap().begin(), itend = getIdMap().end(); it != itend; ++it)
                if (!it->second.isNull()) return false;
        return true;
    }

    bool operator==(const TMultiVecId& v) const
    {
        return defaultId == v.defaultId && getIdMap() == v.getIdMap();
    }

    bool operator!=(const TMultiVecId& v) const
    {
        return !(*this == v);
    }
};

typedef TMultiVecId<V_ALL, V_READ>      ConstMultiVecId;
typedef TMultiVecId<V_ALL, V_WRITE>          MultiVecId;

#if !defined(SOFA_CORE_MULTIVECID_TEMPLATE_DEFINITION)
extern template class SOFA_CORE_API  TMultiVecId<V_ALL, V_READ>;
extern template class SOFA_CORE_API  TMultiVecId<V_ALL, V_WRITE>;
#endif

} // namespace sofa::core

