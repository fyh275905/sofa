#include <GL/glew.h>
#include "Scene.h"

#include <sofa/component/init.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/simulation/common/xml/initXml.h>
#include <sofa/simulation/graph/DAGSimulation.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <sofa/helper/system/glut.h>

#include <qqml.h>
#include <QtCore/QCoreApplication>
#include <QVector3D>
#include <QStack>
#include <QTimer>
#include <QString>
#include <QUrl>
#include <QThread>
#include <QSequentialIterable>
#include <QJSValue>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;

SceneComponent::SceneComponent(const Scene* scene, sofa::core::objectmodel::Base* base) : QObject(),
    myScene(scene),
    myBase(base)
{

}

Base* SceneComponent::base()
{
    return const_cast<Base*>(static_cast<const SceneComponent*>(this)->base());
}

const Base* SceneComponent::base() const
{
    // check object existence
    if(myScene && myBase)
    {
        QStack<Node*> nodes;
        nodes.push(myScene->sofaSimulation()->GetRoot().get());

        while(!nodes.empty())
        {
            Node* node = nodes.pop();
            if(myBase == node)
                return myBase;

            for(unsigned int i = 0; i < node->object.size(); ++i)
                if(myBase == node->object[i])
                    return myBase;

            for(unsigned int i = 0; i < node->child.size(); ++i)
                nodes.push(node->child[i].get());
        }
    }

    myBase = 0;
    return myBase;
}

SceneData::SceneData(const SceneComponent* sceneComponent, sofa::core::objectmodel::BaseData* data) : QObject(),
    mySceneComponent(sceneComponent),
    myData(data)
{

}

QVariantMap SceneData::object() const
{
    const BaseData* data = SceneData::data();
    if(data)
        return Scene::dataObject(data);

    return QVariantMap();
}

void SceneData::setValue(const QVariant& value)
{
    BaseData* data = SceneData::data();
    if(data)
        Scene::setDataValue(data, value);
}

BaseData* SceneData::data()
{
    return const_cast<BaseData*>(static_cast<const SceneData*>(this)->data());
}

const BaseData* SceneData::data() const
{
    // check if the base still exists hence if the data is still valid

    const Base* base = mySceneComponent->base();
    if(!base)
        myData = 0;

    return myData;
}

Scene::Scene(QObject *parent) : QObject(parent),
	myStatus(Status::Null),
	mySource(),
	mySourceQML(),
	myIsInit(false),
    myVisualDirty(false),
	myDt(0.04),
	myPlay(false),
	myAsynchronous(true),
	mySofaSimulation(0),
    myStepTimer(new QTimer(this))
{
	// sofa init
	sofa::helper::system::DataRepository.addFirstPath("./");
    sofa::helper::system::DataRepository.addFirstPath("../../share/");
    sofa::helper::system::DataRepository.addFirstPath("../../examples/");

    sofa::helper::system::PluginRepository.addFirstPath("./");
    sofa::helper::system::PluginRepository.addFirstPath("../bin/");
    sofa::helper::system::PluginRepository.addFirstPath("../lib/");
    sofa::helper::system::PluginRepository.addFirstPath(QCoreApplication::applicationDirPath().toStdString() + "/../bin/");
    sofa::helper::system::PluginRepository.addFirstPath(QCoreApplication::applicationDirPath().toStdString() + "/../lib/");

	sofa::core::ExecParams::defaultInstance()->setAspectID(0);
	boost::shared_ptr<sofa::core::ObjectFactory::ClassEntry> classVisualModel;
	sofa::core::ObjectFactory::AddAlias("VisualModel", "OglModel", true, &classVisualModel);

	myStepTimer->setInterval(0);
	mySofaSimulation = sofa::simulation::graph::getSimulation();

	sofa::component::init();
	sofa::simulation::xml::initXml();

	// plugins
	QVector<QString> plugins;
	plugins.append("SofaPython");

    for(const QString& plugin : plugins)
    {
        std::string s = plugin.toStdString();
        sofa::helper::system::PluginManager::getInstance().loadPlugin(s);
    }

	sofa::helper::system::PluginManager::getInstance().init();

	// connections
	connect(this, &Scene::sourceChanged, this, &Scene::open);
	connect(this, &Scene::playChanged, myStepTimer, [&](bool newPlay) {newPlay ? myStepTimer->start() : myStepTimer->stop();});
	connect(this, &Scene::statusChanged, this, [&](Scene::Status newStatus) {if(Scene::Status::Ready == newStatus) loaded();});

    connect(myStepTimer, &QTimer::timeout, this, &Scene::step);
}

Scene::~Scene()
{
	if(mySofaSimulation == sofa::simulation::getSimulation())
		sofa::simulation::setSimulation(0);
}

static bool LoaderProcess(sofa::simulation::Simulation* sofaSimulation, const QString& scenePath)
{
	if(!sofaSimulation || scenePath.isEmpty())
		return false;

	sofa::core::visual::VisualParams* vparams = sofa::core::visual::VisualParams::defaultInstance();
	if(vparams)
		vparams->displayFlags().setShowVisualModels(true);

	if(sofaSimulation->load(scenePath.toLatin1().constData()))
	{
		sofaSimulation->init(sofaSimulation->GetRoot().get());

		if(sofaSimulation->GetRoot())
			return true;
	}

	return false;
}

class LoaderThread : public QThread
{
public:
	LoaderThread(sofa::simulation::Simulation* sofaSimulation, const QString& scenePath) :
		mySofaSimulation(sofaSimulation),
		myScenepath(scenePath),
		myIsLoaded(false)
	{

	}

	void run()
	{
		myIsLoaded = LoaderProcess(mySofaSimulation, myScenepath);
	}

	bool isLoaded() const			{return myIsLoaded;}

private:
	sofa::simulation::Simulation*	mySofaSimulation;
	QString							myScenepath;
	bool							myIsLoaded;

};

void Scene::open()
{
	setSourceQML(QUrl());

	if(Status::Loading == myStatus) // return now if a scene is already loading
		return;

	QString finalFilename = mySource.toLocalFile();
	if(finalFilename.isEmpty())
	{
		setStatus(Status::Error);
		return;
	}

	std::string filepath = finalFilename.toLatin1().constData();
	if(sofa::helper::system::DataRepository.findFile(filepath))
		finalFilename = filepath.c_str();

	if(finalFilename.isEmpty())
	{
		setStatus(Status::Error);
		return;
	}

	finalFilename.replace("\\", "/");

    aboutToUnload();

	setStatus(Status::Loading);

	setPlay(false);
	myIsInit = false;

	std::string qmlFilepath = (finalFilename + ".qml").toLatin1().constData();
	if(!sofa::helper::system::DataRepository.findFile(qmlFilepath))
		qmlFilepath.clear();

    mySofaSimulation->unload(mySofaSimulation->GetRoot());

	if(myAsynchronous)
	{
		LoaderThread* loaderThread = new LoaderThread(mySofaSimulation, finalFilename);
        connect(loaderThread, &QThread::finished, this, [this, loaderThread]() {setStatus(loaderThread->isLoaded() ? Status::Ready : Status::Error);});
		
		if(!qmlFilepath.empty())
			connect(loaderThread, &QThread::finished, this, [=]() {setSourceQML(QUrl::fromLocalFile(qmlFilepath.c_str()));});

		connect(loaderThread, &QThread::finished, loaderThread, &QObject::deleteLater);
		loaderThread->start();
	}
	else
	{
        setStatus(LoaderProcess(mySofaSimulation, finalFilename) ? Status::Ready : Status::Error);

		if(!qmlFilepath.empty())
			setSourceQML(QUrl::fromLocalFile(qmlFilepath.c_str()));
	}
}

void Scene::setStatus(Status newStatus)
{
	if(newStatus == myStatus)
		return;

	myStatus = newStatus;

	statusChanged(newStatus);
}

void Scene::setSource(const QUrl& newSource)
{
	if(newSource == mySource || Status::Loading == myStatus)
		return;

	setStatus(Status::Null);

	mySource = newSource;

	sourceChanged(newSource);
}

void Scene::setSourceQML(const QUrl& newSourceQML)
{
	if(newSourceQML == mySourceQML)
		return;

	mySourceQML = newSourceQML;

	sourceQMLChanged(newSourceQML);
}

void Scene::setDt(double newDt)
{
	if(newDt == myDt)
		return;

	myDt = newDt;

	dtChanged(newDt);
}

void Scene::setPlay(bool newPlay)
{
	if(newPlay == myPlay)
		return;

	myPlay = newPlay;

	playChanged(newPlay);
}

void Scene::setVisualDirty(bool newVisualDirty)
{
    if(newVisualDirty == myVisualDirty)
        return;

    myVisualDirty = newVisualDirty;

    visualDirtyChanged(newVisualDirty);
}

double Scene::radius() const
{
	QVector3D min, max;
	computeBoundingBox(min, max);
	QVector3D diag = (max - min);

	return diag.length();
}

void Scene::computeBoundingBox(QVector3D& min, QVector3D& max) const
{
	SReal pmin[3], pmax[3];
    mySofaSimulation->computeTotalBBox(mySofaSimulation->GetRoot().get(), pmin, pmax);

	min = QVector3D(pmin[0], pmin[1], pmin[2]);
	max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

QString Scene::dumpGraph() const
{
	QString dump;

	if(mySofaSimulation->GetRoot())
	{
		std::streambuf* backup(std::cout.rdbuf());

		std::ostringstream stream;
		std::cout.rdbuf(stream.rdbuf());
		mySofaSimulation->print(mySofaSimulation->GetRoot().get());
		std::cout.rdbuf(backup);

		dump += QString::fromStdString(stream.str());
	}

	return dump;
}

QVariantMap Scene::dataObject(const sofa::core::objectmodel::BaseData* data)
{
    QVariantMap object;

    if(!data)
        return object;

    // TODO:
    QString type;
    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();

    QVariantMap properties;

    if(typeinfo->Text())
    {
        type = "string";
    }
    else if(typeinfo->Scalar())
    {
        type = "number";
        properties.insert("step", 0.1);
        properties.insert("decimals", 3);
    }
    else if(typeinfo->Integer())
    {
        if(std::string::npos != typeinfo->name().find("bool"))
        {
            type = "boolean";
        }
        else
        {
            type = "number";
            properties.insert("decimals", 0);
            if(std::string::npos != typeinfo->name().find("unsigned"))
                properties.insert("min", 0);
        }
    }

    if(typeinfo->Container())
    {
        type = "array";
        int nbCols = typeinfo->size();

        properties.insert("cols", nbCols);
        if(typeinfo->FixedSize())
            properties.insert("static", true);

        const AbstractTypeInfo* baseTypeinfo = typeinfo->BaseType();
        if(baseTypeinfo->FixedSize())
            properties.insert("innerStatic", true);
    }

    QString widget(data->getWidget());
    if(!widget.isEmpty())
        type = widget;

    object.insert("name", data->getName().c_str());
    object.insert("type", type);
    object.insert("group", data->getGroup());
    object.insert("properties", properties);
    object.insert("value", dataValue(data));

    return object;
}

QVariant Scene::dataValue(const BaseData* data)
{
    QVariant value;

    if(!data)
        return value;

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();
    const void* valueVoidPtr = data->getValueVoidPtr();

    if(!typeinfo->Container())
    {
        if(typeinfo->Text())
            value = QString::fromStdString(typeinfo->getTextValue(valueVoidPtr, 0));
        else if(typeinfo->Scalar())
            value = typeinfo->getScalarValue(valueVoidPtr, 0);
        else if(typeinfo->Integer())
        {
            if(std::string::npos != typeinfo->name().find("bool"))
                value = 0 != typeinfo->getIntegerValue(valueVoidPtr, 0) ? true : false;
            else
                value = typeinfo->getIntegerValue(valueVoidPtr, 0);
        }
    }
    else
    {
        int nbCols = typeinfo->size();
        int nbRows = typeinfo->size(data->getValueVoidPtr()) / nbCols;

        if(typeinfo->Text())
        {
            QVariantList values;
            values.reserve(nbRows);

            QVariantList subValues;
            subValues.reserve(nbCols);

            for(int j = 0; j < nbRows; j++)
            {
                subValues.clear();
                for(int i = 0; i < nbCols; i++)
                    subValues.append(QVariant::fromValue(QString::fromStdString((typeinfo->getTextValue(valueVoidPtr, j * nbCols + i)))));

                values.append(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else if(typeinfo->Scalar())
        {
            QVariantList values;
            values.reserve(nbRows);

            QVariantList subValues;
            subValues.reserve(nbCols);

            for(int j = 0; j < nbRows; j++)
            {
                subValues.clear();
                for(int i = 0; i < nbCols; i++)
                    subValues.append(QVariant::fromValue(typeinfo->getScalarValue(valueVoidPtr, j * nbCols + i)));

                values.append(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else if(typeinfo->Integer())
        {
            QVariantList values;
            values.reserve(nbRows);

            QVariantList subValues;
            subValues.reserve(nbCols);

            bool isBool = false;
            if(std::string::npos != typeinfo->name().find("bool"))
                isBool = true;

            for(int j = 0; j < nbRows; j++)
            {
                subValues.clear();

                if(isBool)
                    for(int i = 0; i < nbCols; i++)
                        subValues.append(QVariant::fromValue(0 != typeinfo->getIntegerValue(valueVoidPtr, j * nbCols + i) ? true : false));
                else
                    for(int i = 0; i < nbCols; i++)
                        subValues.append(QVariant::fromValue(typeinfo->getIntegerValue(valueVoidPtr, j * nbCols + i)));

                values.push_back(QVariant::fromValue(subValues));
            }

            value = values;
        }
    }

    return value;
}

void Scene::setDataValue(BaseData* data, const QVariant& value)
{
    if(!data)
        return;

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();

    if(!value.isNull())
    {
        QVariant finalValue = value;
        if(finalValue.userType() == qMetaTypeId<QJSValue>())
            finalValue = finalValue.value<QJSValue>().toVariant();

        if(QVariant::List == finalValue.type())
        {
            QSequentialIterable valueIterable = finalValue.value<QSequentialIterable>();

            int nbCols = typeinfo->size();
            int nbRows = typeinfo->size(data->getValueVoidPtr()) / nbCols;

            if(!typeinfo->Container())
            {
                qWarning("Trying to set a list of values on a non-container data");
                return;
            }

            if(valueIterable.size() != nbRows)
            {
                if(typeinfo->FixedSize())
                {
                    qWarning() << "The new data should have the same size, should be" << nbRows << ", got" << valueIterable.size();
                    return;
                }

                typeinfo->setSize(data, valueIterable.size());
            }

            if(typeinfo->Scalar())
            {
                QString dataString;
                for(int i = 0; i < valueIterable.size(); ++i)
                {
                    QVariant subFinalValue = valueIterable.at(i);
                    if(QVariant::List == subFinalValue.type())
                    {
                        QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                        if(subValueIterable.size() != nbCols)
                        {
                            qWarning() << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                            return;
                        }

                        for(int j = 0; j < subValueIterable.size(); ++j)
                        {
                            dataString += QString::number(subValueIterable.at(j).toDouble());
                            if(subValueIterable.size() - 1 != j)
                                dataString += ' ';
                        }
                    }
                    else
                    {
                        dataString += QString::number(subFinalValue.toDouble());
                    }

                    if(valueIterable.size() - 1 != i)
                        dataString += ' ';
                }

                data->read(dataString.toStdString());
            }
            else if(typeinfo->Integer())
            {
                QString dataString;
                for(int i = 0; i < valueIterable.size(); ++i)
                {
                    QVariant subFinalValue = valueIterable.at(i);
                    if(QVariant::List == subFinalValue.type())
                    {
                        QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                        if(subValueIterable.size() != nbCols)
                        {
                            qWarning("The new sub data should have the same size");
                            return;
                        }

                        for(int j = 0; j < subValueIterable.size(); ++j)
                        {
                            dataString += QString::number(subValueIterable.at(j).toLongLong());
                            if(subValueIterable.size() - 1 != j)
                                dataString += ' ';
                        }
                    }
                    else
                    {
                        dataString += QString::number(subFinalValue.toLongLong());
                    }

                    if(valueIterable.size() - 1 != i)
                        dataString += ' ';
                }

                data->read(dataString.toStdString());
            }
        }
        else if(QVariant::Map == finalValue.type())
        {
            qWarning("Map type are not supported");
        }
        else
        {
            if(typeinfo->Text())
                data->read(value.toString().toStdString());
            else if(typeinfo->Scalar())
                data->read(QString::number(value.toDouble()).toStdString());
            else if(typeinfo->Integer())
                data->read(QString::number(value.toLongLong()).toStdString());
        }
    }
}

QVariant Scene::dataValue(const QString& path) const
{
    return onDataValue(path);
}

void Scene::setDataValue(const QString& path, const QVariant& value)
{
    onSetDataValue(path, value);
}

QVariant Scene::onDataValue(const QString& path) const
{
    BaseData* data = 0;
    mySofaSimulation->GetRoot()->findDataLinkDest(data, path.toStdString(), 0);
    if(!data)
    {
        qWarning() << "DataPath unknown:" << path;
        return QVariant();
    }

    return dataValue(data);
}

void Scene::onSetDataValue(const QString& path, const QVariant& value)
{
    BaseData* data = 0;
    mySofaSimulation->GetRoot()->findDataLinkDest(data, path.toStdString(), 0);

    if(!data)
    {
        qWarning() << "DataPath unknown:" << path;
    }
    else
    {
        if(!value.isNull())
        {
            QVariant finalValue = value;
            if(finalValue.userType() == qMetaTypeId<QJSValue>())
                finalValue = finalValue.value<QJSValue>().toVariant();

            // arguments from JS are packed in an array, we have to unpack it
            if(QVariant::List == finalValue.type())
            {
                QSequentialIterable valueIterable = finalValue.value<QSequentialIterable>();
                if(1 == valueIterable.size())
                    finalValue = valueIterable.at(0);
            }

            setDataValue(data, finalValue);
        }
    }
}

void Scene::init()
{
	if(!mySofaSimulation->GetRoot())
		return;

    GLenum err = glewInit();
    if(0 != err)
        qWarning() << "GLEW Initialization failed with error code:" << err;

    // prepare the sofa visual params
    sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
    if(visualParams)
    {
        if(!visualParams->drawTool())
        {
            visualParams->drawTool() = new sofa::core::visual::DrawToolGL();
            visualParams->setSupported(sofa::core::visual::API_OpenGL);
        }
    }

#ifdef __linux__
    static bool glutInited = false;
    if(!glutInited)
    {
        int argc = 0;
        glutInit(&argc, NULL);
        glutInited = true;
    }
#endif

	mySofaSimulation->initTextures(mySofaSimulation->GetRoot().get());
	setDt(mySofaSimulation->GetRoot()->getDt());

    myIsInit = true;
}

void Scene::reload()
{
    // TODO: ! NEED CURRENT OPENGL CONTEXT while releasing the old sofa scene
    //qDebug() << "reload - thread" << QThread::currentThread() << QOpenGLContext::currentContext() << (void*) &glLightfv;

    open();
}

void Scene::step()
{
	if(!mySofaSimulation->GetRoot())
		return;

	emit stepBegin();
    mySofaSimulation->animate(mySofaSimulation->GetRoot().get(), myDt);
    setVisualDirty(true);
    emit stepEnd();
}

void Scene::reset()
{
    if(!mySofaSimulation->GetRoot())
        return;

    // TODO: ! NEED CURRENT OPENGL CONTEXT
    mySofaSimulation->reset(mySofaSimulation->GetRoot().get());
    setVisualDirty(true);
    emit reseted();
}

void Scene::draw()
{
	if(!mySofaSimulation->GetRoot())
		return;

    // prepare the sofa visual params
    sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
    if(visualParams)
    {
        GLint _viewport[4];
        GLdouble _mvmatrix[16], _projmatrix[16];

        glGetIntegerv(GL_VIEWPORT, _viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, _mvmatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, _projmatrix);

        visualParams->viewport() = sofa::helper::fixed_array<int, 4>(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
        visualParams->sceneBBox() = mySofaSimulation->GetRoot()->f_bbox.getValue();
        visualParams->setProjectionMatrix(_projmatrix);
        visualParams->setModelViewMatrix(_mvmatrix);
    }

    //qDebug() << "draw - thread" << QThread::currentThread() << QOpenGLContext::currentContext();

    if(visualDirty())
    {
        mySofaSimulation->updateVisual(mySofaSimulation->GetRoot().get());
        setVisualDirty(false);
	}

	mySofaSimulation->draw(sofa::core::visual::VisualParams::defaultInstance(), mySofaSimulation->GetRoot().get());
}

void Scene::onKeyPressed(char key)
{
	if(!mySofaSimulation->GetRoot())
		return;

	sofa::core::objectmodel::KeypressedEvent keyEvent(key);
	sofaSimulation()->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void Scene::onKeyReleased(char key)
{
	if(!mySofaSimulation->GetRoot())
		return;

	sofa::core::objectmodel::KeyreleasedEvent keyEvent(key);
	sofaSimulation()->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

}

}
