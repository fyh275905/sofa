/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2019 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_CORE_DATATRACKER_H
#define SOFA_CORE_DATATRACKER_H

#include <sofa/core/objectmodel/DDGNode.h>

namespace sofa
{

namespace core
{

    /// Tracking Data mechanism
    /// to be able to check when selected Data changed since their last clean.
    ///
    /// The Data must be added to tracking system by calling "trackData".
    /// Then it can be checked if it changed with "hasChanged" since its last "clean".
    /// 
    /// Use datatrackers to check if your data have changed! Do not use 
    /// BaseData's "isDirty()" method, as it has a completely different purpose:
    /// BaseData::isDirty() checks whether or not the data is up-to-date with its
    /// parent values while DataTracker::hasChanged(myData) checks whether the data
    /// has been modified since it has last been checked 
    struct SOFA_CORE_API DataTracker
    {
        /// select a Data to track to be able to check
        /// if it was dirtied since the previous clean.
        /// @see isTrackedDataDirty
        void trackData( const objectmodel::BaseData& data );

        /// Did the data change since its last access?
        /// @warning data must be a tracked Data @see trackData
        bool hasChanged( const objectmodel::BaseData& data );

        /// Did one of the tracked data change since the last call to clean()?
        bool hasChanged();

        /// comparison point is cleaned for the specified tracked Data
        /// @warning data must be a tracked Data @see trackData
        void clean( const objectmodel::BaseData& data );

        /// comparison point is cleaned for all tracked Data
        void clean();

        /// Provide the map of tracked Data
        const std::map<const objectmodel::BaseData*,int>&  getMapTrackedData() {return m_dataTrackers;}

    protected:
        /// map a tracked Data to a DataTracker (storing its call-counter at each 'clean')
        typedef std::map<const objectmodel::BaseData*,int> DataTrackers;
        DataTrackers m_dataTrackers;
    };


/////////////////////////


    /// A DDGNode with trackable input Data (containing a DataTracker)
    class SOFA_CORE_API DataTrackerDDGNode : public core::objectmodel::DDGNode
    {
    public:

        DataTrackerDDGNode() : core::objectmodel::DDGNode() {}

    private:
        DataTrackerDDGNode(const DataTrackerDDGNode&);
        void operator=(const DataTrackerDDGNode&);

    public:
        /// Create a DataCallback object associated with multiple Data.
        void addInputs(std::initializer_list<sofa::core::objectmodel::BaseData*> datas);
        void addOutputs(std::initializer_list<sofa::core::objectmodel::BaseData*> datas);

        /// Set dirty flag to false
        /// for the DDGNode and for all the tracked Data
        virtual void cleanDirty(const core::ExecParams* params = nullptr);


        /// Utility function to ensure all inputs are up-to-date
        /// can be useful for particulary complex DDGNode
        /// with a lot input/output imbricated access
        void updateAllInputsIfDirty();

    protected:
        /// Function adding the data to the DataTracker for inputs
        void trackInputData(const objectmodel::BaseData &data);

        /// Function checking if a specific input data changed
        bool hasInputDataChanged(const objectmodel::BaseData &data);

        /// Function checking if one or several inputs did changed
        bool haveInputsDataChanged();

        /// Function cleaning the DataTracker for inputs
        void cleanInputTracker();

    private:
        /// @name Tracking Data mechanism
        /// each Data added to the DataTracker
        /// is tracked to be able to check if its value changed
        /// since their last clean, called by default
        /// in DataEngine::cleanDirty().
        /// @{
        DataTracker m_inputDataTracker;
        ///@}

    };


/////////////////////////


    /// a DDGNode that automatically triggers its update function
    /// when asking for an output and any input changed.
    /// Similar behavior than a DataEngine, but this is NOT a component
    /// and can be used everywhere.
    ///
    /// Note that it contains a DataTrackerDDGNode (m_dataTracker)
    /// to be able to check precisly which input changed if needed.
    ///
    ///
    ///
    ///
    /// **** Implementation good rules: (similar to DataEngine)
    ///
    /// //init
    ///    addInput // indicate all inputs
    ///    addOutput // indicate all outputs
    ///    setDirtyValue(); // the engine must start dirty (of course, no output are up-to-date)
    ///
    ///
    /// void UpdateCallback( DataTrackerEngine* dataTrackerEngine )
    /// {
    ///      // get the list of inputs for this DDGNode
    ///      const core::DataTrackerEngine::DDGLinkContainer& inputs = dataTrackerEngine->getInputs();
    ///      // get the list of outputs for this DDGNode
    ///      const core::DataTrackerEngine::DDGLinkContainer& outputs = dataTrackerEngine->getOutputs();
    ///
    ///      // we known who is who from the order Data were added to the DataTrackerEngine
    ///      static_cast<Data< FirstInputType >*>( inputs[0] );
    ///
    ///      // all inputs must be updated
    ///      // can be done by Data::getValue, ReadAccessor, Data::updateIfDirty, DataTrackerDDGNode::updateAllInputsIfDirty
    ///
    ///      // must be called AFTER updating all inputs, otherwise a modified input will set the engine to dirty again.
    ///      // must be called BEFORE read access to an output, otherwise read-accessing the output will call update
    ///      dataTrackerEngine->cleanDirty();
    ///
    ///      // FINALLY access and set outputs
    ///      // Note that a write-only access has better performance and is enough in 99% engines   Data::beginWriteOnly, WriteOnlyAccessor
    ///      // A read access is possible, in that case, be careful the cleanDirty is called before the read-access
    /// }
    ///
    class SOFA_CORE_API DataTrackerEngine : public DataTrackerDDGNode
    {
    public:
        /// set the update function to call
        /// when asking for an output and any input changed.
        [[deprecated("This function has been replaced by addCallback with similar signature. Update your code.")]]
        void setUpdateCallback(std::function<void(DataTrackerEngine*)> f){ addCallback(f); }

        /// set the update function to call
        /// when asking for an output and any input changed.
        void addCallback(std::function<void(DataTrackerEngine*)> f);

        /// Calls the callback when one of the data has changed.
        void update() override;

        /// This method is needed by DDGNode
        const std::string& getName() const override
        {
            static const std::string emptyName ="";
            return emptyName;
        }
        /// This method is needed by DDGNode
        objectmodel::Base* getOwner() const override { return nullptr; }
        /// This method is needed by DDGNode
        objectmodel::BaseData* getData() const override { return nullptr; }

    protected:
        std::vector<std::function<void(DataTrackerEngine*)>> m_callbacks;
    };

} // namespace core

} // namespace sofa

#endif
