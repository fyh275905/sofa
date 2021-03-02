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
#ifndef SOFA_CORE_OBJECTMODEL_BASE_H
#define SOFA_CORE_OBJECTMODEL_BASE_H

#include <sofa/defaulttype/BoundingBox.h>                    //NOTE-FOR-SELF: Remove
#include <sofa/core/objectmodel/Data.h>
#include <sofa/core/objectmodel/Link.h>                      //NOTE-FOR-SELF: Remove
#include <sofa/core/objectmodel/BaseClass.h>
#include <sofa/core/objectmodel/BaseObjectDescription.h>     //NOTE-FOR-SELF: Remove
#include <sofa/core/objectmodel/Tag.h>
#include <list>
#include <sofa/core/sptr.h>

#include <deque>

#include <sofa/core/objectmodel/ComponentState.h>
#include <sofa/core/DataTracker.h>

// forward declaration of castable classes
// @author Matthieu Nesme, 2015
// it is not super elegant, but it is way more efficient than dynamic_cast
namespace sofa {
namespace core {
    class BaseState;
    class BaseMapping;
    class BehaviorModel;
    class CollisionModel;
    class DataEngine;
    class DevBaseMonitor;
namespace objectmodel {
    class BaseContext;
    class BaseObject;
    class BaseNode;
    class ContextObject;
    class ConfigurationSetting;
} // namespace objectmodel
namespace behavior {
    class BaseAnimationLoop;
    class OdeSolver;
    class BaseLinearSolver;
    class LinearSolver;
    class ConstraintSolver;
    class BaseMass;
    class BaseMechanicalState;
    class BaseInteractionForceField;
    class BaseInteractionConstraint;
    class BaseForceField;
    class BaseProjectiveConstraintSet;
    class BaseInteractionProjectiveConstraintSet;
    class BaseConstraintSet;
    class BaseConstraint;
} // namespace behavior
namespace visual {
    class VisualModel;
    class VisualManager;
    class VisualLoop;
    class Shader;
} // namespace visual
namespace topology {
    class Topology;
    class BaseMeshTopology;
    class BaseTopologyObject;
} // namespace topology
namespace collision {
    class CollisionGroupManager;
    class ContactManager;
    class Detection;
    class Intersection;
    class Pipeline;
} // namespace collision
namespace loader
{
    class BaseLoader;
} // namespace loader
} // namespace core
} // namespace sofa

// VisitorScheduler

// Empty class to be used to highlight deprecated objects at compilation time.
class DeprecatedAndRemoved {};


#define SOFA_BASE_CAST_IMPLEMENTATION(CLASSNAME) \
virtual const CLASSNAME* to##CLASSNAME() const override { return this; } \
virtual       CLASSNAME* to##CLASSNAME()       override { return this; }

namespace sofa
{

namespace core
{

namespace objectmodel
{

/**
 *  \brief Base class for everything
 *
 *  This class contains all functionnality shared by every objects in SOFA.
 *  Most importantly it defines how to retrieve information about an object (name, type, data fields).
 *  All classes deriving from Base should use the SOFA_CLASS macro within their declaration (see BaseClass.h).
 *
 */
class SOFA_CORE_API Base
{
public:
    typedef Base* Ptr;

    using SPtr = sptr<Base>;
    
    using MyClass = TClass< Base, void >;
    static const BaseClass* GetClass() { return MyClass::get(); }
    virtual const BaseClass* getClass() const { return GetClass(); }

protected:
    /// Constructor cannot be called directly
    /// Use the New() method instead
    Base();



    /// Direct calls to destructor are forbidden.
    /// Smart pointers must be used to manage creation/destruction of objects
    virtual ~Base();

private:
    /// Copy constructor is not allowed
    Base(const Base& b);
    Base& operator=(const Base& b);

    std::atomic<int> ref_counter;
    void addRef();
    void release();

    friend inline void intrusive_ptr_add_ref(Base* p)
    {
        p->addRef();
    }

    friend inline void intrusive_ptr_release(Base* p)
    {
        p->release();
    }

protected:
    std::map<std::string, sofa::core::DataTrackerCallback> m_internalEngine;

public:
    void addUpdateCallback(const std::string& name,
                           std::initializer_list<BaseData*> inputs,
                           std::function<sofa::core::objectmodel::ComponentState(const DataTracker&)> function,
                           std::initializer_list<BaseData*> outputs);
    void addOutputsToCallback(const std::string& name, std::initializer_list<BaseData*> outputs);


    virtual std::string getPathName() const { return ""; }

    /// Accessor to the object name
    const std::string& getName() const
    {
        return name.getValue();
    }

    /// Set the name of this object
    void setName(const std::string& n);

    /// Set the name of this object, adding an integer counter
    void setName(const std::string& n, int counter);

    /// Get the type name of this object (i.e. class and template types)
    /// Since #PR 1283, the signature has changed to "final" so it is not possible
    /// to override the getTypeName() method.
    virtual std::string getTypeName() const ;

    /// Get the class name of this object
    /// Since #PR 1283, the signature has changed to "final" so it is not possible
    /// to override the getClassName() method. To specify custom class name you need
    /// to implement a single static std::string GetCustomClassName(){} method.
    virtual std::string getClassName() const ;

    /// Get the template type names (if any) used to instantiate this object
    /// Since #PR 1283, the signature has changed to "final" so it is not possible
    /// to override the getClassName() method. To specify custom class name you need
    /// to implement a single static std::string GetCustomTemplateName(){} method.
    virtual std::string getTemplateName() const ;

    /// Get the template type names (if any) used to instantiate this object
    /// Since #PR 1283, the signature has changed to "final" so it is not possible
    /// to override the getNameSpaceName() method.
    virtual std::string getNameSpaceName() const ;

    /// Set the source filename (where the component is implemented)
    void setDefinitionSourceFileName(const std::string& sourceFileName);

    /// Get the source filename (where the component is implemented)
    const std::string& getDefinitionSourceFileName() const;

    /// Set the source location (where the component is implemented)
    void setDefinitionSourceFilePos(const int);

    /// Get the source location (where the component is implemented)
    int getDefinitionSourceFilePos() const;

    /// Set the file where the instance has been created
    /// This is useful to store where the component was emitted from
    void setInstanciationSourceFileName(const std::string& sourceFileName);

    /// Get the file where the instance has been created
    /// This is useful to store where the component was emitted from
    const std::string& getInstanciationSourceFileName() const;

    /// Set the file location (line number) where the instance has been created
    /// This is useful to store where the component was emitted from
    void setInstanciationSourceFilePos(const int);

    /// Get the file location (line number) where the instance has been created
    /// This is useful to store where the component was emitted from
    int getInstanciationSourceFilePos() const;

    /// @name fields
    ///   Data fields management
    /// @{

    /// Assign one field value (Data or Link)
    virtual bool parseField( const std::string& attribute, const std::string& value);

    /// Check if a given Data field or Link exists
    virtual bool hasField( const std::string& attribute) const;

    /// Parse the given description to assign values to this object's fields and potentially other parameters
    virtual void parse ( BaseObjectDescription* arg );

    /// Assign the field values stored in the given list of name + value pairs of strings
    void parseFields ( const std::list<std::string>& str );

    /// Assign the field values stored in the given map of name -> value pairs
    virtual void parseFields ( const std::map<std::string,std::string*>& str );

    /// Write the current field values to the given map of name -> value pairs
    void writeDatas (std::map<std::string,std::string*>& str);

    /// Write the current field values to the given output stream
    /// separated with the given separator (" " used by default for XML)
    void writeDatas (std::ostream& out, const std::string& separator = " ");

    /// Find a data field given its name. Return nullptr if not found.
    /// If more than one field is found (due to aliases), only the first is returned.
    BaseData* findData( const std::string &name ) const;



    /// Find data fields given a name: several can be found as we look into the alias map
    std::vector< BaseData* > findGlobalField( const std::string &name ) const;

    /// Find a link given its name. Return nullptr if not found.
    /// If more than one link is found (due to aliases), only the first is returned.
    BaseLink* findLink( const std::string &name ) const;

    /// Find link fields given a name: several can be found as we look into the alias map
    std::vector< BaseLink* > findLinks( const std::string &name ) const;

    /// Update pointers in case the pointed-to objects have appeared
    virtual void updateLinks(bool logErrors = true);

    /// Helper method used to initialize a data field containing a value of type T
    template<class T>
    BaseData::BaseInitData initData( Data<T>* field, const char* name, const char* help, bool isDisplayed=true, bool isReadOnly=false )
    {
        BaseData::BaseInitData res;
        this->initData0(field, res, name, help, isDisplayed, isReadOnly);
        return res;
    }

    /// Helper method used to initialize a data field containing a value of type T
    template<class T>
    typename Data<T>::InitData initData( Data<T>* field, const T& value, const char* name, const char* help, bool isDisplayed=true, bool isReadOnly=false  )
    {
        typename Data<T>::InitData res;
        this->initData0(field, res, value, name, help, isDisplayed, isReadOnly);
        return res;
    }

    /// Add a data field.
    /// Note that this method should only be called if the Data was not initialized with the initData method
    void addData(BaseData* f, const std::string& name);


    /// Add a data field.
    /// Note that this method should only be called if the Data was not initialized with the initData method
    void addData(BaseData* f);

    /// Remove a data field.
    void removeData(BaseData* f);


    /// Add an alias to a Data
    void addAlias( BaseData* field, const char* alias);

    /// Add a link.
    void addLink(BaseLink* l);

    /// Add an alias to a Link
    void addAlias( BaseLink* link, const char* alias);


    typedef helper::vector<BaseData*> VecData;
    typedef std::multimap<std::string, BaseData*> MapData;

    typedef helper::vector<BaseLink*> VecLink;
    typedef std::multimap<std::string, BaseLink*> MapLink;


    /// Accessor to the vector containing all the fields of this object
    const VecData& getDataFields() const { return m_vecData; }
    /// Accessor to the map containing all the aliases of this object
    const MapData& getDataAliases() const { return m_aliasData; }

    /// Accessor to the vector containing all the fields of this object
    const VecLink& getLinks() const { return m_vecLink; }
    /// Accessor to the map containing all the aliases of this object
    const MapLink& getLinkAliases() const { return m_aliasLink; }

    virtual bool findDataLinkDest(BaseData*& ptr, const std::string& path, const BaseLink* link);
    virtual Base* findLinkDestClass(const BaseClass* destType, const std::string& path, const BaseLink* link);
    template<class T>
    bool findLinkDest(T*& ptr, const std::string& path, const BaseLink* link)
    {
        Base* result = findLinkDestClass(T::GetClass(), path, link);
        ptr = dynamic_cast<T*>(result);
        return (result != nullptr);
    }

    /// @}

    /// @name tags
    ///   Methods related to tagged subsets
    /// @{

    /// Represents the subsets the object belongs to
    const sofa::core::objectmodel::TagSet& getTags() const { return f_tags.getValue(); }

    /// Return true if the object belong to the given subset
    bool hasTag( Tag t ) const;

    /// Add a subset qualification to the object
    void addTag(Tag t);
    /// Remove a subset qualification to the object
    void removeTag(Tag t);

    /// @}


private:
    /// effective ostringstream for logging
    mutable std::ostringstream _serr, _sout;
    mutable std::deque<sofa::helper::logging::Message> m_messageslog ;

public:
    /// write into component buffer + Message processedby message handlers
    /// default message type = Warning
    mutable helper::system::SofaOStream<helper::logging::Message::Warning> serr;
    /// write into component buffer.
    /// Message is processed by message handlers only if printLog==true
    /// /// default message type = Info
    mutable helper::system::SofaOStream<helper::logging::Message::Info> sout;
    /// runs the stream processing
    mutable helper::system::SofaEndl<Base> sendl;

    ////////////// DEPRECATED /////////////////////////////////////////////////////////////////////////////
    const std::string getWarnings() const;  /// use getLoggedMessageAsString() or getLoggedMessage instead.
    const std::string getOutputs() const;   /// use getLoggedMessageAsString() or getLoggedMessage instead.
    void clearWarnings();                   /// use clearLoggedMessages() instead
    void clearOutputs();                    /// use clearLoggedMessages() instead
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    void processStream(std::ostream& out);

    void addMessage(const sofa::helper::logging::Message& m) const ;
    size_t  countLoggedMessages(sofa::helper::logging::Message::TypeSet t=sofa::helper::logging::Message::AnyTypes) const ;
    const std::deque<sofa::helper::logging::Message>& getLoggedMessages() const ;
    const std::string getLoggedMessagesAsString(sofa::helper::logging::Message::TypeSet t=sofa::helper::logging::Message::AnyTypes) const ;

    void clearLoggedMessages() const ;

    inline bool notMuted() const { return f_printLog.getValue(); }

protected:
    /// Helper method used by initData()
    void initData0( BaseData* field, BaseData::BaseInitData& res, const char* name, const char* help, bool isDisplayed=true, bool isReadOnly=false );
    void initData0( BaseData* field, BaseData::BaseInitData& res, const char* name, const char* help, BaseData::DataFlags dataFlags );

    /// Helper method used by initData()
    template<class T>
    void initData0( Data<T>* field, typename Data<T>::InitData& res, const T& value, const char* name, const char* help, bool isDisplayed=true, bool isReadOnly=false )
    {
        initData0( field, res, name, help, isDisplayed, isReadOnly );
        res.value = value;
    }



public:

    /// Helper method & type for the NameDecoder class to it can detect inherited instances
    /// The following code is only needed since #PR1283 to smooth the deprecation process.
    /// Remove that after the 01.01.2021.
    bool IsInheritingFromBase(){return true;}
    typedef Base BaseType;

    /// Helper method to get the type name of a type derived from this class
    ///
    /// This method should be used as follow :
    /// \code  T* ptr = nullptr; std::string type = T::typeName(ptr); \endcode
    /// This way derived classes can redefine the typeName method
    template<class T>
    SOFA_ATTRIBUTE_DEPRECATED__CLASSNAME_INTROSPECTION()
    static std::string typeName(const T* ptr = nullptr)
    {
        SOFA_UNUSED(ptr);
        return sofa::helper::NameDecoder::decodeTypeName(typeid(T));
    }

    /// Helper method to get the class name of a type derived from this class
    ///
    /// This method should be used as follow :
    /// \code  std::string type = Base::className<B>(); \endcode
    /// This way derived classes can redefine the className method
    template<class T>
    SOFA_ATTRIBUTE_DEPRECATED__CLASSNAME_INTROSPECTION()
    static std::string className(const T* ptr = nullptr)
    {
        SOFA_UNUSED(ptr);
        return sofa::helper::NameDecoder::decodeClassName(typeid(T));
    }

    /// Helper method to get the namespace name of a type derived from this class
    ///
    /// This method should be used as follow :
    /// \code  std::string type = Base::namespaceName<T>(); \endcode
    /// This way derived classes can redefine the namespaceName method
    template<class T>
    SOFA_ATTRIBUTE_DEPRECATED__CLASSNAME_INTROSPECTION()
    static std::string namespaceName(const T* ptr = nullptr)
    {
        SOFA_UNUSED(ptr);
        return sofa::helper::NameDecoder::decodeNamespaceName(typeid(T));
    }

    /// Helper method to get the template name of a type derived from this class
    ///
    /// This method should be used as follow :
    /// \code  std::string type = Base::templateName<B>); \endcode
    /// This way derived classes can redefine the templateName method
    template<class T>
    SOFA_ATTRIBUTE_DEPRECATED__CLASSNAME_INTROSPECTION()
    static std::string templateName(const T* ptr = nullptr)
    {
        SOFA_UNUSED(ptr);
        return sofa::helper::NameDecoder::decodeTemplateName(typeid(T));
    }

    /// Helper method to get the shortname of a type derived from this class.
    /// The default implementation return the class name.
    ///
    /// This method should be used as follow :
    /// \code  std::string type = Base::shortNam<B>(); \endcode
    /// This way derived classes can redefine the shortName method
    template< class T>
    static std::string shortName(const T* ptr = nullptr, BaseObjectDescription* = nullptr )
    {
        SOFA_UNUSED(ptr);
        return sofa::helper::NameDecoder::shortName(sofa::helper::NameDecoder::getClassName<T>());
    }

    /// @name componentstate
    ///   Methods related to component state
    /// @{

    ComponentState getComponentState() const { return d_componentState.getValue() ; }
    bool isComponentStateValid() const { return d_componentState.getValue() == ComponentState::Valid; }

    ///@}


protected:
    /// List of fields (Data instances)
    VecData m_vecData;
    /// name -> Data multi-map (includes names and aliases)
    MapData m_aliasData;

    /// List of links
    VecLink m_vecLink;
    /// name -> Link multi-map (includes names and aliases)
    MapLink m_aliasLink;

public:
    /// Name of the object.
    Data<std::string> name;


    Data<bool> f_printLog; ///< if true, emits extra messages at runtime.

    Data< sofa::core::objectmodel::TagSet > f_tags; ///< list of the subsets the objet belongs to

    Data< sofa::defaulttype::BoundingBox > f_bbox; ///< this object bounding box

    Data< sofa::core::objectmodel::ComponentState >  d_componentState; ///< the object state

    SOFA_ATTRIBUTE_DISABLED__COMPONENTSTATE("To fix your code, use d_componentState")
    DeprecatedAndRemoved m_componentstate;

    SOFA_ATTRIBUTE_DISABLED__COMPONENTSTATE("To fix your code, use d_componentState")
    DeprecatedAndRemoved d_componentstate;

    std::string m_definitionSourceFileName        {""};
    int         m_definitionSourceFilePos         {-1};
    std::string m_instanciationSourceFileName     {""};
    int         m_instanciationSourceFilePos      {-1};

    /// @name casting
    ///   trivial cast to a few base components
    ///   through virtual functions
    ///   returns nullptr by default
    ///   must be specialized in each type implementation to return a pointer of this type
    /// @{
    ///
public:



#define SOFA_BASE_CAST_DEFINITION(NAMESPACE,CLASSNAME) \
    virtual const NAMESPACE::CLASSNAME* to##CLASSNAME() const { return nullptr; } \
    virtual       NAMESPACE::CLASSNAME* to##CLASSNAME()       { return nullptr; }

    SOFA_BASE_CAST_DEFINITION( core,        BaseState                              )
    SOFA_BASE_CAST_DEFINITION( core,        BaseMapping                            )
    SOFA_BASE_CAST_DEFINITION( core,        BehaviorModel                          )
    SOFA_BASE_CAST_DEFINITION( core,        CollisionModel                         )
    SOFA_BASE_CAST_DEFINITION( core,        DataEngine                             )
    SOFA_BASE_CAST_DEFINITION( core,        DevBaseMonitor                         )
    SOFA_BASE_CAST_DEFINITION( objectmodel, BaseContext                            )
    SOFA_BASE_CAST_DEFINITION( objectmodel, BaseObject                             )
    SOFA_BASE_CAST_DEFINITION( objectmodel, BaseNode                               )
    SOFA_BASE_CAST_DEFINITION( objectmodel, ContextObject                          )
    SOFA_BASE_CAST_DEFINITION( objectmodel, ConfigurationSetting                   )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseAnimationLoop                      )
    SOFA_BASE_CAST_DEFINITION( behavior,    OdeSolver                              )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseLinearSolver                       )
    SOFA_BASE_CAST_DEFINITION( behavior,    LinearSolver                           )
    SOFA_BASE_CAST_DEFINITION( behavior,    ConstraintSolver                       )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseMass                               )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseMechanicalState                    )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseInteractionForceField              )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseInteractionConstraint              )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseForceField                         )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseProjectiveConstraintSet            )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseInteractionProjectiveConstraintSet )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseConstraintSet                      )
    SOFA_BASE_CAST_DEFINITION( behavior,    BaseConstraint                         )
    SOFA_BASE_CAST_DEFINITION( visual,      VisualModel                            )
    SOFA_BASE_CAST_DEFINITION( visual,      VisualManager                          )
    SOFA_BASE_CAST_DEFINITION( visual,      VisualLoop                             )
    SOFA_BASE_CAST_DEFINITION( visual,      Shader                                 )
    SOFA_BASE_CAST_DEFINITION( topology,    Topology                               )
    SOFA_BASE_CAST_DEFINITION( topology,    BaseMeshTopology                       )
    SOFA_BASE_CAST_DEFINITION( topology,    BaseTopologyObject                     )
    SOFA_BASE_CAST_DEFINITION( collision,   CollisionGroupManager                  )
    SOFA_BASE_CAST_DEFINITION( collision,   ContactManager                         )
    SOFA_BASE_CAST_DEFINITION( collision,   Detection                              )
    SOFA_BASE_CAST_DEFINITION( collision,   Intersection                           )
    SOFA_BASE_CAST_DEFINITION( collision,   Pipeline                               )
    SOFA_BASE_CAST_DEFINITION( loader,      BaseLoader                             )

#undef SOFA_BASE_CAST_DEFINITION

    /// @}
};


} // namespace objectmodel

} // namespace core

} // namespace sofa

/// This allow Base object to interact with the messaging system.
namespace sofa
{
namespace helper
{
namespace logging
{
    inline bool notMuted(const sofa::core::objectmodel::Base* t){ return t->notMuted(); }
    inline bool notMuted(sofa::core::objectmodel::Base* t){ return t->notMuted(); }

    class SOFA_CORE_API SofaComponentInfo : public ComponentInfo
    {
    public:
        const sofa::core::objectmodel::Base* m_component ;
        std::string                          m_name;

        SofaComponentInfo(const sofa::core::objectmodel::Base* c);
        const std::string& name() const { return m_name; }
        std::ostream& toStream(std::ostream &out) const
        {
            out << m_sender << "(" << m_name << ")" ;
            return out ;
        }
    };

    /// This construct a new ComponentInfo object from a Base object.
    inline ComponentInfo::SPtr getComponentInfo(const sofa::core::objectmodel::Base* t)
    {
        return ComponentInfo::SPtr( new SofaComponentInfo(t) ) ;
    }
} // logging
} // helper
} // sofa
#endif
