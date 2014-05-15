/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_CONFIGURATIONSETTING_VIEWER_H
#define SOFA_COMPONENT_CONFIGURATIONSETTING_VIEWER_H

#include <sofa/core/objectmodel/ConfigurationSetting.h>
#include <sofa/component/component.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/OptionsGroup.h>

namespace sofa
{

namespace component
{

namespace configurationsetting
{

using namespace sofa::core::objectmodel;
using namespace sofa::defaulttype;
using namespace sofa::helper;

///Class for the configuration of viewer settings.
class SOFA_GRAPH_COMPONENT_API ViewerSetting: public ConfigurationSetting
{
public:
    SOFA_CLASS(ViewerSetting,ConfigurationSetting); ///< Sofa macro to define typedef.
protected:

    /**
     * @brief Default constructor.
     *
     * By default :
     *  - @ref resolution is set to 800x600.
     *  - @ref fullscreen is set to false.
     *  - @ref cameraMode is set to projective.
     *  - @ref objectPickingMethod is set to ray casting.
     */
    ViewerSetting();
public:

    Data<Vec<2,int> > resolution;                           ///< Screen resolution (width, height).
    Data<bool> fullscreen;                                  ///< True if viewer should be fullscreen.
    Data<OptionsGroup> cameraMode;                          ///< Camera mode.
                                                            /**<    \arg Perspective.
                                                             *      \arg Orthographic.
                                                             */
    Data<OptionsGroup> objectPickingMethod;                 ///< Picking Method.
                                                            /**<    \arg Ray casting.
                                                             *      \arg Selection Buffer.
                                                             */
};

}

}

}
#endif
