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

#include <SofaBaseUtils/RequiredPlugin.h>

#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/helper/system/FileSystem.h>
using sofa::helper::system::FileSystem;
#include <sofa/helper/logging/Messaging.h>

using sofa::helper::system::PluginManager;

namespace sofa::component::misc
{

int RequiredPluginClass = core::RegisterObject("Load the required plugins")
        .add< RequiredPlugin >();

RequiredPlugin::RequiredPlugin()
    : d_pluginName( initData(&d_pluginName, "pluginName", "plugin name (or several names if you need to load different plugins or a plugin with several alternate names)"))
    , d_suffixMap ( initData(&d_suffixMap , "suffixMap", "standard->custom suffixes pairs (to be used if the plugin is compiled outside of Sofa with a non standard way of differenciating versions), using ! to represent empty suffix"))
    , d_stopAfterFirstNameFound( initData(&d_stopAfterFirstNameFound , false, "stopAfterFirstNameFound", "Stop after the first plugin name that is loaded successfully"))
    , d_stopAfterFirstSuffixFound( initData(&d_stopAfterFirstSuffixFound , true, "stopAfterFirstSuffixFound", "For each plugin name, stop after the first suffix that is loaded successfully"))
    , d_requireOne ( initData(&d_requireOne , false, "requireOne", "Display an error message if no plugin names were successfully loaded"))
    , d_requireAll ( initData(&d_requireAll , true, "requireAll", "Display an error message if any plugin names failed to be loaded"))
    , d_loadedPlugins(initData(&d_loadedPlugins, "loadedPlugins", "List of the plugins that are have been loaded."))
{
    this->f_printLog.setValue(true); // print log by default, to identify which pluging is responsible in case of a crash during loading

    /// Add a callback to update the required plugin when its data are changed
    addUpdateCallback("reloadPlugins", {&name,
                                    &d_pluginName, &d_suffixMap, &d_stopAfterFirstNameFound, &d_stopAfterFirstSuffixFound,
                                    &d_requireAll, &d_requireOne},
                      [this](const sofa::core::DataTracker&)
    {
        clearLoggedMessages();
        /// Reload the plugins and check at least one is loaded.
        if(loadPlugin())
            return sofa::core::objectmodel::ComponentState::Valid;
        return sofa::core::objectmodel::ComponentState::Invalid;
    }, {&d_loadedPlugins});
}

void RequiredPlugin::parse(sofa::core::objectmodel::BaseObjectDescription* arg)
{
    d_componentState = sofa::core::objectmodel::ComponentState::Invalid;

    Inherit1::parse(arg);
    if(loadPlugin())
        d_componentState = sofa::core::objectmodel::ComponentState::Valid;
}

bool RequiredPlugin::loadPlugin()
{
    /// Get a write accessor to the loadedPlugin
    auto loadedPlugins = sofa::helper::getWriteOnlyAccessor(d_loadedPlugins);
    loadedPlugins.clear();

    sofa::helper::system::PluginManager* pluginManager = &sofa::helper::system::PluginManager::getInstance();
    const std::string defaultSuffix = PluginManager::getDefaultSuffix();
    const helper::vector<helper::fixed_array<std::string,2> >& sMap = d_suffixMap.getValue();
    helper::vector<std::string> suffixVec;
    if (!sMap.empty())
    {
        std::string skey = (defaultSuffix.empty() ? std::string("!") : defaultSuffix);
        for (std::size_t i = 0; i < sMap.size(); ++i)
        {
            if (sMap[i][0] == skey)
            {
                suffixVec.push_back(sMap[i][1] == std::string("!") ? std::string(""):sMap[i][1]);
            }
        }
    }
    if (suffixVec.empty())
        suffixVec.push_back(defaultSuffix);

    /// Copy the lost of names provided as arguments
    const helper::vector<std::string>& nameVec = d_pluginName.getValue();
    helper::vector<std::string> pluginsToLoad = nameVec;

    /// In case the pluginName is not set we copy the provided name into the set to load.
    if(!d_pluginName.isSet() && name.isSet())
    {
        pluginsToLoad.push_back(this->getName());
    }

    helper::vector< std::string > failed;
    std::ostringstream errmsg;
    for (auto& pluginName : pluginsToLoad)
    {
        const std::string& name = FileSystem::cleanPath( pluginName ); // name is not necessarily a path
        bool nameLoaded = false;
        for (auto& suffix : suffixVec)
        {
            if ( pluginManager->pluginIsLoaded(name) )
            {
                loadedPlugins.push_back(name);
                nameLoaded = true;
                if (d_stopAfterFirstSuffixFound.getValue()) break;
            }
            else if ( pluginManager->loadPlugin(name, suffix, true, true, &errmsg) )
            {
                loadedPlugins.push_back(name);
                nameLoaded = true;
                if (d_stopAfterFirstSuffixFound.getValue()) break;
            }
        }
        if (!nameLoaded)
        {
            failed.push_back(name);
        }
        else if (d_stopAfterFirstNameFound.getValue())
        {
            break;
        }
    }

    bool hasFailed=false;
    if (!failed.empty())
    {
        if ((d_requireAll.getValue() || (d_requireOne.getValue() && loadedPlugins.empty())))
        {
            hasFailed = true;
            msg_error() << errmsg.str() << msgendl
                        << "Failed to load: " << failed ;
        }
        else
        {
            msg_warning() << errmsg.str() << msgendl
                          << "Unable to load optional: " << failed;
        }
    }
    pluginManager->init();
    return !hasFailed;
}

} // namespace sofa::component::misc
