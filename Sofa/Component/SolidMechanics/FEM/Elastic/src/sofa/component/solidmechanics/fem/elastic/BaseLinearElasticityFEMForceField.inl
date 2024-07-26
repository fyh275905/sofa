﻿/******************************************************************************
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
#include <sofa/component/solidmechanics/fem/elastic/BaseLinearElasticityFEMForceField.h>
#include <sofa/core/behavior/ForceField.inl>

namespace sofa::component::solidmechanics::fem::elastic
{

template <class DataTypes>
typename BaseLinearElasticityFEMForceField<DataTypes>::VecReal
BaseLinearElasticityFEMForceField<DataTypes>::GetDefaultYoungModulusValue()
{
    VecReal newY;
    newY.resize(1);
    newY[0] = 5000;
    return newY;
}

template <class DataTypes>
BaseLinearElasticityFEMForceField<DataTypes>::BaseLinearElasticityFEMForceField()
    : d_poissonRatio(initData(&d_poissonRatio, defaultPoissonRatioValue, "poissonRatio", "FEM Poisson Ratio in Hooke's law [0,0.5["))
    , d_youngModulus(initData(&d_youngModulus, defaultVecYoungModulusValue, "youngModulus", "FEM Young's Modulus in Hooke's law"))
    , l_topology(initLink("topology", "link to the topology container"))
{
    d_poissonRatio.setRequired(true);
    d_poissonRatio.setWidget("poissonRatio");

    d_youngModulus.setRequired(true);

    this->addUpdateCallback("checkPoissonRatio", {&d_poissonRatio}, [this](const core::DataTracker& )
    {
        checkPoissonRatio();
        return sofa::core::objectmodel::ComponentState::Valid;
    }, {});

    this->addUpdateCallback("checkPositiveYoungModulus", {&d_youngModulus}, [this](const core::DataTracker& )
    {
        checkYoungModulus();
        return sofa::core::objectmodel::ComponentState::Valid;
    }, {});
}

template <class DataTypes>
void BaseLinearElasticityFEMForceField<DataTypes>::checkPoissonRatio()
{
    const auto& poissonRatio = d_poissonRatio.getValue();
    if (poissonRatio < 0 || poissonRatio >= 0.5)
    {
        msg_warning() << "Poisson's ratio must be in the range [0, 0.5), "
                "but an out-of-bounds value has been provided (" <<
                poissonRatio << "). It is set to " << defaultPoissonRatioValue <<
                " to ensure the correct behavior";
        d_poissonRatio.setValue(defaultPoissonRatioValue);
    }
}

template <class DataTypes>
void BaseLinearElasticityFEMForceField<DataTypes>::checkYoungModulus()
{
    auto youngModulus = sofa::helper::getWriteAccessor(d_youngModulus);
    for (auto& y : youngModulus)
    {
        if (y < 0)
        {
            msg_warning() << "Young's modulus must be positive, but "
                    "a negative value has been provided (" << y <<
                    "). It is set to " << defaultYoungModulusValue <<
                    " to ensure the correct behavior";
            y = defaultYoungModulusValue;
        }
    }
}

template <class DataTypes>
void BaseLinearElasticityFEMForceField<DataTypes>::init()
{
    core::behavior::ForceField<DataTypes>::init();

    if (l_topology.empty())
    {
        msg_info() << "link to Topology container should be set to ensure right behavior. First Topology found in current context will be used.";
        l_topology.set(this->getContext()->getMeshTopologyLink());
    }

    checkYoungModulus();
    checkPoissonRatio();
}

template <class DataTypes>
void BaseLinearElasticityFEMForceField<DataTypes>::setPoissonRatio(Real val)
{
    this->d_poissonRatio.setValue(val);
}

template <class DataTypes>
void BaseLinearElasticityFEMForceField<DataTypes>::setYoungModulus(Real val)
{
    VecReal newY;
    newY.resize(1);
    newY[0] = val;
    d_youngModulus.setValue(newY);
}

template <class DataTypes>
auto BaseLinearElasticityFEMForceField<DataTypes>::getYoungModulusInElement(sofa::Size elementId)
-> Real
{
    Real youngModulusElement {};

    const auto& youngModulus = d_youngModulus.getValue();
    if (youngModulus.size() > elementId)
    {
        youngModulusElement = youngModulus[elementId];
    }
    else if (!youngModulus.empty())
    {
        youngModulusElement = youngModulus[0];
    }
    else
    {
        setYoungModulus(5000);
        youngModulusElement = youngModulus[0];
    }
    return youngModulusElement;
}

template <class DataTypes>
typename BaseLinearElasticityFEMForceField<DataTypes>::Real
BaseLinearElasticityFEMForceField<DataTypes>::getPoissonRatioInElement(
    sofa::Size elementId)
{
    return d_poissonRatio.getValue();
}

template <class DataTypes>
auto BaseLinearElasticityFEMForceField<DataTypes>::toLameParameters(
    const ElementsType2D elementType,
    const Real youngModulus,
    const Real poissonRatio) -> std::pair<Real, Real>
{
    SOFA_UNUSED(elementType);

    //Lamé's first parameter
    const Real lambda = youngModulus * poissonRatio / (1 - poissonRatio * poissonRatio);

    //Lamé's second parameter (or shear modulus)
    const Real mu = youngModulus / (2 * (1 + poissonRatio));

    return {lambda, mu};
}

template <class DataTypes>
auto BaseLinearElasticityFEMForceField<DataTypes>::toLameParameters(
    const ElementsType3D elementType,
    const Real youngModulus,
    const Real poissonRatio) -> std::pair<Real, Real>
{
    SOFA_UNUSED(elementType);

    //Lamé's first parameter
    const Real lambda = youngModulus * poissonRatio / ((1 - 2 * poissonRatio) * (1 + poissonRatio));

    //Lamé's second parameter (or shear modulus)
    const Real mu = youngModulus / (2 * (1 + poissonRatio));

    return {lambda, mu};
}

}
