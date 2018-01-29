/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_SERVERCOMMUNICATION_H
#define SOFA_SERVERCOMMUNICATION_H


#include <Communication/config.h>

#include <sofa/core/ObjectFactory.h>

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject ;

#include <sofa/core/objectmodel/Data.h>
using sofa::core::objectmodel::Data;
using sofa::core::objectmodel::BaseData;

#include <sofa/helper/vectorData.h>
using sofa::helper::vectorData;
using sofa::helper::WriteAccessorVector;
using sofa::helper::WriteAccessor;
using sofa::helper::ReadAccessor;

#include <sofa/helper/OptionsGroup.h>
using sofa::helper::OptionsGroup;

#include <sofa/core/objectmodel/Event.h>
#include <sofa/simulation/AnimateBeginEvent.h>
using sofa::core::objectmodel::Event;
using sofa::simulation::AnimateBeginEvent ;

#include <sofa/defaulttype/DataTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo ;

using sofa::helper::vector;

#include <SofaBaseLinearSolver/FullMatrix.h>
using sofa::component::linearsolver::FullMatrix;

#include <sofa/helper/Factory.h>
#include <sofa/helper/Factory.inl>
using sofa::helper::Factory;

#include <sofa/helper/system/thread/CircularQueue.h>

#include <pthread.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <mutex>
#include <cmath>

namespace sofa
{

namespace component
{

namespace communication
{

//forward declaration
class CommunicationSubscriber;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef std::vector<std::string> ArgumentList;

class BufferData
{
public:
    BufferData(){}
    BufferData(SingleLink<CommunicationSubscriber, BaseObject, BaseLink::FLAG_DOUBLELINK> source,
    CommunicationSubscriber * subscriber,
    std::string subject,
    ArgumentList argumentList,
    int rows,
    int cols)
    {
        this->source = source;
        this->subscriber = subscriber;
        this->subject = subject;
        this->argumentList = argumentList;
        this->rows = rows;
        this->cols = cols;
    }

    SingleLink<CommunicationSubscriber, BaseObject, BaseLink::FLAG_DOUBLELINK> source;
    CommunicationSubscriber * subscriber;
    std::string subject;
    ArgumentList argumentList;
    int rows ;
    int cols ;
};

class CircularBuffer
{
public:
    CircularBuffer(int size)
    {
        this->data[size] = {};
        this->size = size;
    }

    ~CircularBuffer()
    {
        delete this->data;
    }

    void add(
            SingleLink<CommunicationSubscriber, BaseObject, BaseLink::FLAG_DOUBLELINK> source,
            CommunicationSubscriber * subscriber,
            std::string subject,
            ArgumentList argumentList,
            int rows ,
            int cols)
    {
        if (isFull())
            throw std::out_of_range("Circular buffer is full");
        pthread_mutex_lock(&mutex);
        data[rear] = new BufferData(source, subscriber, subject, argumentList, rows, cols);
        rear = ((this->rear + 1) % this->size);
        pthread_mutex_unlock(&mutex);
    }

    BufferData* get()
    {
        if (isEmpty())
            throw std::out_of_range("Circular buffer is empty");
        pthread_mutex_lock(&mutex);
        BufferData* aData = this->data[front];
        front = (front + 1) % size;
        pthread_mutex_unlock(&mutex);
        return aData;
    }

    bool isEmpty()
    {
        return rear == front;
    }

    bool isFull()
    {
        return ((this->rear + 1) % this->size) == front;
    }

private:
    int front = 0;
    int rear = 0;
    BufferData ** data;
    int size;
};


template<typename DataType>
class DataCreator : public sofa::helper::BaseCreator<BaseData>
{
public:
    virtual BaseData* createInstance(sofa::helper::NoArgument) override { return new sofa::core::objectmodel::Data<DataType>(); }
    virtual const std::type_info& type() override { return typeid(BaseData);}
};

class SOFA_COMMUNICATION_API ServerCommunication : public BaseObject
{

public:

    typedef BaseObject Inherited;
    SOFA_ABSTRACT_CLASS(ServerCommunication, Inherited);

    ServerCommunication() ;
    virtual ~ServerCommunication() ;

    bool isSubscribedTo(std::string, unsigned int);
    void addSubscriber(CommunicationSubscriber*);
    std::map<std::string, CommunicationSubscriber*> getSubscribers();
    CommunicationSubscriber* getSubscriberFor(std::string);

    bool isRunning() { return m_running;}
    void setRunning(bool value) {m_running = value;}

    virtual std::string getArgumentType(std::string argument) =0;
    virtual std::string getArgumentValue(std::string argument) =0;

    //////////////////////////////// Factory type /////////////////////////////////
    typedef sofa::helper::Factory< std::string, BaseData> CommunicationDataFactory;
    virtual CommunicationDataFactory* getFactoryInstance() =0;
    virtual void initTypeFactory() =0;
    /////////////////////////////////////////////////////////////////////////////////

    ////////////////////////// Inherited from BaseObject ////////////////////
    virtual void init() override;
    virtual void handleEvent(Event *) override;
    /////////////////////////////////////////////////////////////////////////

    bool saveArgumentsToBuffer(
            SingleLink<CommunicationSubscriber, BaseObject, BaseLink::FLAG_DOUBLELINK> source,
            CommunicationSubscriber * subscriber,
            std::string subject,
            ArgumentList argumentList,
            int rows ,
            int cols );
    BufferData* fetchArgumentsFromBuffer();

    Data<helper::OptionsGroup>  d_job;
    Data<std::string>           d_address;
    Data<int>                   d_port;
    Data<double>                d_refreshRate;

protected:

    CircularBuffer* receiveDataBuffer = new CircularBuffer(3);
    std::map<std::string, CommunicationSubscriber*> m_subscriberMap;
    pthread_t                                       m_thread;
    bool                                            m_running = true;

    virtual void openCommunication();
    virtual void closeCommunication();
    static void* thread_launcher(void*);
    virtual void sendData() =0;
    virtual void receiveData() =0;


    BaseData* fetchData(SingleLink<CommunicationSubscriber,  BaseObject, BaseLink::FLAG_DOUBLELINK> source, std::string keyTypeMessage, std::string argumentName);
    bool writeData(BufferData* data);
    bool writeDataToContainer(BufferData* data);
    bool writeDataToFullMatrix(BufferData* data);

};

} /// namespace communication
} /// namespace component
} /// namespace sofa

#endif // SOFA_SERVERCOMMUNICATION_H
