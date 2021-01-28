#pragma once
#include <sofa/core/config.h>
#include <iosfwd>

namespace sofa::core
{
class BaseMapping;
}

namespace sofa::core::objectmodel
{
class Base;
class BaseObject;
class BaseNode;
class BaseContext;
class BaseData;
class BaseLink;

class Tag;
SOFA_CORE_API std::ostream& operator<<(std::ostream& o, const Tag& t);
SOFA_CORE_API std::istream& operator>>(std::istream& i, Tag& t);
}

namespace sofa::core::behavior
{
class BaseForceField;
class BaseMass;
class BaseMechanicalState;
}

namespace sofa::core::topology
{
class TopologyChange;
SOFA_CORE_API std::ostream& operator<< ( std::ostream& out, const sofa::core::topology::TopologyChange* t );
SOFA_CORE_API std::istream& operator>> ( std::istream& in, sofa::core::topology::TopologyChange*& t );
SOFA_CORE_API std::istream& operator>> ( std::istream& in, const sofa::core::topology::TopologyChange*& );
}

namespace sofa::core::visual
{
class VisualParams;

class FlagTreeItem;
SOFA_CORE_API std::ostream& operator<< ( std::ostream& os, const FlagTreeItem& root );
SOFA_CORE_API std::istream& operator>> ( std::istream& in, FlagTreeItem& root );

class DislpayFlags;
SOFA_CORE_API std::ostream& operator<< ( std::ostream& os, const DisplayFlags& flags );
SOFA_CORE_API std::istream& operator>> ( std::istream& in, DisplayFlags& flags );
}

namespace sofa::component::topology
{
class TetrahedronSetTopologyContainer;
SOFA_CORE_API std::ostream& operator<< (std::ostream& out, const TetrahedronSetTopologyContainer& t);
SOFA_CORE_API std::istream& operator>>(std::istream& in, TetrahedronSetTopologyContainer& t);
}
