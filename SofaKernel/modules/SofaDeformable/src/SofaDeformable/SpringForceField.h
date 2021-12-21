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
#include <SofaDeformable/config.h>

#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/type/vector.h>
#include <sofa/helper/accessor.h>

#include <sofa/core/objectmodel/DataFileName.h>

#include <SofaSimulationGraph/DAGNode.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBaseMechanics/SubsetMultiMapping.h>

namespace sofa::component::interactionforcefield
{

/// This class contains the description of one linear spring
template<class T>
class LinearSpring
{
public:
    typedef T Real;
    sofa::Index  m1, m2;    ///< the two extremities of the spring: masses m1 and m2
    Real ks;                ///< spring stiffness
    Real kd;                ///< damping factor
    Real initpos;           ///< rest length of the spring
    bool elongationOnly;    ///< only forbid elongation, not compression
    bool enabled;           ///< false to disable this spring (i.e. broken)

    LinearSpring(sofa::Index m1=0, sofa::Index m2=0, Real ks=0.0, Real kd=0.0, Real initpos=0.0, bool noCompression=false, bool enabled=true)
        : m1(m1), m2(m2), ks(ks), kd(kd), initpos(initpos), elongationOnly(noCompression), enabled(enabled)
    {
    }

    inline friend std::istream& operator >> ( std::istream& in, LinearSpring<Real>& s )
    {
        in>>s.m1>>s.m2>>s.ks>>s.kd>>s.initpos;
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const LinearSpring<Real>& s )
    {
        out<<s.m1<<" "<<s.m2<<" "<<s.ks<<" "<<s.kd<<" "<<s.initpos<<"\n";
        return out;
    }

};

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class SpringForceFieldInternalData
{
public:
};

/// Set of simple springs between particles
template<class DataTypes>
class SpringForceField : public core::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(SpringForceField,DataTypes), SOFA_TEMPLATE(core::behavior::ForceField,DataTypes));

    typedef typename core::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef core::objectmodel::Data<VecDeriv>    DataVecDeriv;
    typedef core::objectmodel::Data<VecCoord>    DataVecCoord;

    typedef helper::ReadAccessor< Data< VecCoord > > RDataRefVecCoord;
    typedef helper::WriteAccessor< Data< VecCoord > > WDataRefVecCoord;
    typedef helper::ReadAccessor< Data< VecDeriv > > RDataRefVecDeriv;
    typedef helper::WriteAccessor< Data< VecDeriv > > WDataRefVecDeriv;

    typedef core::behavior::MechanicalState<DataTypes> MechanicalState;

    typedef LinearSpring<Real> Spring;

    Data<SReal> ks; ///< uniform stiffness for the all springs
    Data<SReal> kd; ///< uniform damping for the all springs
    Data<float> showArrowSize; ///< size of the axis
    Data<int> drawMode;             ///Draw Mode: 0=Line - 1=Cylinder - 2=Arrow
    Data<sofa::type::vector<Spring> > springs; ///< pairs of indices, stiffness, damping, rest length

protected:
    core::objectmodel::DataFileName fileSprings;

protected:
    bool maskInUse;
    Real m_potentialEnergy;
    class Loader;

    SpringForceFieldInternalData<DataTypes> data;
    friend class SpringForceFieldInternalData<DataTypes>;

    virtual void addSpringForce(Real& potentialEnergy, VecDeriv& f1, const VecCoord& p1, const VecDeriv& v1, VecDeriv& f2, const VecCoord& p2, const VecDeriv& v2, sofa::Index /*i*/, const Spring& spring);

    SpringForceField(SReal _ks=100.0, SReal _kd=5.0);
    SpringForceField(MechanicalState* mstate, SReal _ks=100.0, SReal _kd=5.0);

public:
    bool load(const char *filename);

    const sofa::type::vector< Spring >& getSprings() const {return springs.getValue();}

    void reinit() override;
    void init() override;

    void addForce(const core::MechanicalParams* /*mparams*/, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v) override;
    void addDForce(const core::MechanicalParams* mparams, DataVecDeriv& df, const DataVecDeriv& dx ) override;

    SReal getPotentialEnergy(const core::MechanicalParams* /*mparams*/, const DataVecCoord& x) const override;

    using Inherit::addKToMatrix;
    virtual void addKToMatrix(sofa::linearalgebra::BaseMatrix * /*mat*/, SReal /*kFact*/, unsigned int &/*offset*/);

    SReal getStiffness() const { return ks.getValue(); }
    SReal getDamping() const { return kd.getValue(); }
    void setStiffness(SReal _ks) { ks.setValue(_ks); }
    void setDamping(SReal _kd) { kd.setValue(_kd); }
    SReal getArrowSize() const {return showArrowSize.getValue();}
    void setArrowSize(float s) {showArrowSize.setValue(s);}
    int getDrawMode() const {return drawMode.getValue();}
    void setDrawMode(int m) {drawMode.setValue(m);}

    void draw(const core::visual::VisualParams* vparams) override;

    // -- Modifiers

    void clear(sofa::Size reserve=0)
    {
        sofa::type::vector<Spring>& springs = *this->springs.beginEdit();
        springs.clear();
        if (reserve) springs.reserve(reserve);
        this->springs.endEdit();
    }

    void removeSpring(sofa::Index idSpring)
    {
        if (idSpring >= (this->springs.getValue()).size())
            return;

        sofa::type::vector<Spring>& springs = *this->springs.beginEdit();
        springs.erase(springs.begin() +idSpring );
        this->springs.endEdit();
    }

    void addSpring(sofa::Index m1, sofa::Index m2, SReal ks, SReal kd, SReal initlen)
    {
        springs.beginEdit()->push_back(Spring(m1,m2,ks,kd,initlen));
        springs.endEdit();
    }

    void addSpring(const Spring & spring)
    {
        springs.beginEdit()->push_back(spring);
        springs.endEdit();
    }

    void handleTopologyChange(core::topology::Topology *topo) override;


    protected:
    /// stream to export Potential Energy to gnuplot files
    std::ofstream* m_gnuplotFileEnergy;

public:

    /**
     * To facilitate the design of a simulation scene, this component accepts two Data attributes ('object1' and
     * 'object2') corresponding to two different objects. This is to create springs between two distincts objects, as
     * it is very common situtation.
     * If 'object1' and 'object2' are provided, this function makes sure the links to 'object1' and 'object2'
     * are compatible with T.
     */
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        // To make creation of springs between two objects possible, this class accepts two Data attributes ('object1'
        // and 'object2') which do not corresponds to any actual Data.

        const std::string object1 = arg->getAttribute("object1","");
        const std::string object2 = arg->getAttribute("object2","");

        if (object1 == object2)
        {
            if (!object1.empty())
            {
                arg->setAttribute("mstate", object1);
                arg->removeAttribute("object1");
                arg->removeAttribute("object2");
            }
            return Inherit1::canCreate(obj, context, arg);
        }

        sofa::core::behavior::MechanicalState<DataTypes>* mstate1 = nullptr;
        sofa::core::behavior::MechanicalState<DataTypes>* mstate2 = nullptr;

        context->findLinkDest(mstate1, object1, nullptr);
        context->findLinkDest(mstate2, object2, nullptr);

        if (!mstate1)
        {
            arg->logError("Data attribute 'object1' does not point to a valid mechanical state of datatype '" + std::string(DataTypes::Name()) + "'.");
            return false;
        }
        if (!mstate2)
        {
            arg->logError("Data attribute 'object2' does not point to a valid mechanical state of datatype '" + std::string(DataTypes::Name()) + "'.");
            return false;
        }

        if (!dynamic_cast<simulation::graph::DAGNode*>(context))
        {
            arg->logError("Context where this object is created is not a DAGNode. Other contexts than DAGNode are not supported.");
            return false;
        }

        //at this stage, object1 and object2 are two distinct and compatible objects. The SpringForceField can be created.
        //But the mapping, linking the two objects, remains to be created in the create function

        return core::objectmodel::BaseObject::canCreate(obj, context, arg);
    }

    /**
     * The creation of a SpringForceField (or a derived class), depends on whether 'object1' and 'object2' are
     * provided or not.
     * If they are provided, the creation is more complex as it involves the creation of a single force field for two
     * distinct objects. This is done using a SubsetMultiMapping.
     * @sa CreateSpringBetweenObjects
     */
    template<class T>
    static typename T::SPtr create(T* obj, sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription* arg)
    {
        const std::string object1 = arg->getAttribute("object1","");
        const std::string object2 = arg->getAttribute("object2","");

        arg->removeAttribute("object1");
        arg->removeAttribute("object2");

        if (object1 != object2)
        {
            sofa::core::behavior::MechanicalState<DataTypes>* mstate1 = nullptr;
            sofa::core::behavior::MechanicalState<DataTypes>* mstate2 = nullptr;

            context->findLinkDest(mstate1, object1, nullptr);
            context->findLinkDest(mstate2, object2, nullptr);

            typename T::SPtr createdObject = sofa::core::objectmodel::New<T>();

            if (context)
            {
                if (auto* node = dynamic_cast<simulation::graph::DAGNode*>(context))
                {
                    const std::string mergeName = "merge_" + mstate1->getName() + "-" + mstate2->getName();
                    const std::string springName = arg ? arg->getAttribute("name", mergeName.c_str()) : mergeName;
                    const auto createdNode = node->createChild(springName);

                    auto mstate = core::objectmodel::New<sofa::component::container::MechanicalObject<DataTypes> >();
                    mstate->setName(mergeName);
                    createdNode->addObject(mstate);

                    auto mapping = core::objectmodel::New<mapping::SubsetMultiMapping<DataTypes, DataTypes> >();
                    mapping->setName("multiMapping");
                    createdNode->addObject(mapping);

                    mapping->addInputModel(mstate1);
                    mapping->addInputModel(mstate2);
                    mapping->addOutputModel(mstate.get());

                    if (arg) createdObject->parse(arg);

                    auto springs = sofa::helper::getWriteAccessor(createdObject->springs);
                    auto indexPairs = sofa::helper::getWriteAccessor(mapping->indexPairs);
                    sofa::Index mstateIndex {};
                    for (auto& spring : springs)
                    {
                        indexPairs->push_back(0);
                        indexPairs->push_back(spring.m1);
                        spring.m1 = mstateIndex++;

                        indexPairs->push_back(1);
                        indexPairs->push_back(spring.m2);
                        spring.m2 = mstateIndex++;
                    }

                    createdNode->addObject(createdObject);
                }
                else
                {
                    context->addObject(createdObject);
                    if (arg) createdObject->parse(arg);
                }
            }

            return createdObject;
        }

        return Inherit1::create(obj, context, arg);
    }


};

/**
 * Utilitary function to create a series of springs between nodes of two objects. A Node is created in the provided
 * context. In this Node, 3 components are inserted: 1) a new mechanical state which will be the fusion of the provided
 * objects, 2) a SubsetMultiMapping that will make the link between the two provided objects and the new mechanical state,
 * and 3) the spring force field.
 * @tparam SpringForceFieldType Type of force field that will be created. SpringForceField or a derived class is expected. The
 * data types are deduced from SpringForceFieldType.
 * @param context The context where all the components will be inserted
 * @param mstate1 The mechanical state of the first object
 * @param mstate2 The mechanical state of the second object
 * @param springsRelativeToBothObjects A list of springs where the indices refer to nodes in mstate1 and mstate2
 * @return A tuple containing the created Node and the 3 created components, which are inserted into the created Node
 */
template<class SpringForceFieldType>
static std::tuple<
    simulation::Node::SPtr,
    typename sofa::component::container::MechanicalObject<typename SpringForceFieldType::DataTypes>::SPtr,
    typename mapping::SubsetMultiMapping<typename SpringForceFieldType::DataTypes, typename SpringForceFieldType::DataTypes>::SPtr,
    typename SpringForceFieldType::SPtr>
CreateSpringBetweenObjects(
    sofa::core::objectmodel::BaseContext* context,
    sofa::core::behavior::MechanicalState<typename SpringForceFieldType::DataTypes>* mstate1,
    sofa::core::behavior::MechanicalState<typename SpringForceFieldType::DataTypes>* mstate2,
    const sofa::type::vector<LinearSpring<typename SpringForceFieldType::Real> >& springsRelativeToBothObjects)
{
    using DataTypes = typename SpringForceFieldType::DataTypes;
    typename SpringForceFieldType::SPtr createdObject = sofa::core::objectmodel::New<SpringForceFieldType>();

    if (context)
    {
        if (auto* node = dynamic_cast<simulation::graph::DAGNode*>(context))
        {
            const std::string mergeName = "merge_" + mstate1->getName() + "-" + mstate2->getName();
            const auto createdNode = node->createChild(mergeName);

            auto mstate = core::objectmodel::New<sofa::component::container::MechanicalObject<DataTypes> >();
            mstate->setName(mergeName);
            createdNode->addObject(mstate);

            auto mapping = core::objectmodel::New<mapping::SubsetMultiMapping<DataTypes, DataTypes> >();
            mapping->setName("multiMapping");
            createdNode->addObject(mapping);

            mapping->addInputModel(mstate1);
            mapping->addInputModel(mstate2);
            mapping->addOutputModel(mstate.get());

            createdNode->addObject(createdObject);

            auto springs = sofa::helper::getWriteAccessor(createdObject->springs);
            springs.clear();

            auto indexPairs = sofa::helper::getWriteAccessor(mapping->indexPairs);

            sofa::Index mstateIndex {};
            for (const auto& spring : springsRelativeToBothObjects)
            {
                auto springRelativeToNewMstate = spring;

                indexPairs->push_back(0);
                indexPairs->push_back(spring.m1);
                springRelativeToNewMstate.m1 = mstateIndex++;

                indexPairs->push_back(1);
                indexPairs->push_back(spring.m2);
                springRelativeToNewMstate.m2 = mstateIndex++;

                springs.push_back(springRelativeToNewMstate);
            }

            return {createdNode, mstate, mapping, createdObject};
        }
    }
    return {nullptr, nullptr, nullptr, nullptr};
}

#if  !defined(SOFA_COMPONENT_FORCEFIELD_SPRINGFORCEFIELD_CPP)
extern template class SOFA_SOFADEFORMABLE_API LinearSpring<double>;
extern template class SOFA_SOFADEFORMABLE_API SpringForceField<defaulttype::Vec3Types>;
extern template class SOFA_SOFADEFORMABLE_API SpringForceField<defaulttype::Vec2Types>;
extern template class SOFA_SOFADEFORMABLE_API SpringForceField<defaulttype::Vec1Types>;
extern template class SOFA_SOFADEFORMABLE_API SpringForceField<defaulttype::Vec6Types>;
extern template class SOFA_SOFADEFORMABLE_API SpringForceField<defaulttype::Rigid3Types>;
#endif

} // namespace sofa::component::interactionforcefield
