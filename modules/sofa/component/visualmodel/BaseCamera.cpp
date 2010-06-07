/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in theHope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You shouldHave received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L.Heigeas, C. Mendoza,   *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
/*
 * Camera.cpp
 *
 *      Author: froy
 */

#include <sofa/component/visualmodel/BaseCamera.h>

#include <sofa/defaulttype/Mat.h>
#include <sofa/defaulttype/SolidTypes.h>
#include <sofa/helper/gl/Axis.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

BaseCamera::BaseCamera()
    :p_position(initData(&p_position, "position", "Camera's position"))
    ,p_orientation(initData(&p_orientation, "orientation", "Camera's orientation"))
    ,p_lookAt(initData(&p_lookAt, "lookAt", "Camera's look at"))
    ,p_distance(initData(&p_distance, "distance", "Distance between camera and look at"))
    ,p_fieldOfView(initData(&p_fieldOfView, (double) 45.0 , "fieldOfView", "Camera's FOV"))
    ,p_zNear(initData(&p_zNear, (double) 0.1 , "zNear", "Camera's zNear"))
    ,p_zFar(initData(&p_zFar, (double) 1000.0 , "zFar", "Camera's zFar"))
    ,p_minBBox(initData(&p_minBBox, Vec3(0.0,0.0,0.0) , "minBBox", "minBBox"))
    ,p_maxBBox(initData(&p_maxBBox, Vec3(1.0,1.0,1.0) , "maxBBox", "maaxBBox"))
    ,p_widthViewport(initData(&p_widthViewport, (unsigned int) 800 , "widthViewport", "widthViewport"))
    ,p_heightViewport(initData(&p_heightViewport,(unsigned int) 600 , "heightViewport", "heightViewport"))
    ,p_type(initData(&p_type, (int) BaseCamera::PERSPECTIVE_TYPE, "type", "Camera Type (0 = Perspective, 1 = Orthographic)"))
{

}

BaseCamera::~BaseCamera()
{

}


void BaseCamera::translate(const Vec3& t)
{
    Vec3 &pos = *p_position.beginEdit();
    pos += t;
    p_position.endEdit();

}

void BaseCamera::translateLookAt(const Vec3& t)
{
    Vec3 &lookat = *p_lookAt.beginEdit();
    lookat += t;
    currentLookAt = lookat;
    p_lookAt.endEdit();

}

void BaseCamera::rotate(const Quat& r)
{
    Quat &rot = *p_orientation.beginEdit();
    rot = rot * r;
    rot.normalize();
    p_orientation.endEdit();
}

BaseCamera::Vec3 BaseCamera::cameraToWorldCoordinates(const Vec3& p)
{
    return p_orientation.getValue().rotate(p) + p_position.getValue();
}

BaseCamera::Vec3 BaseCamera::worldToCameraCoordinates(const Vec3& p)
{
    return p_orientation.getValue().inverseRotate(p - p_position.getValue());
}

BaseCamera::Vec3 BaseCamera::cameraToWorldTransform(const Vec3& v)
{
    Quat q = p_orientation.getValue();
    return q.rotate(v) ;
}

BaseCamera::Vec3 BaseCamera::worldToCameraTransform(const Vec3& v)
{
    return p_orientation.getValue().inverseRotate(v);
}

void BaseCamera::getOpenGLMatrix(double mat[16])
{
    defaulttype::SolidTypes<double>::Transform world_H_cam(p_position.getValue(), this->getOrientation());
    world_H_cam.inversed().writeOpenGlMatrix(mat);
}

void BaseCamera::init()
{
    if(p_position.isSet())
    {
        if(!p_orientation.isSet())
        {
            p_distance.setValue((p_lookAt.getValue() - p_position.getValue()).norm());

            Quat q  = getOrientationFromLookAt(p_position.getValue(), p_lookAt.getValue());
            p_orientation.setValue(q);
        }
        else if(!p_lookAt.isSet())
        {
            //distance assumed to be set
            if(!p_distance.isSet())
                sout << "Missing distance parameter ; taking default value (0.0, 0.0, 0.0)" << sendl;

            Vec3 lookat = getLookAtFromOrientation(p_position.getValue(), p_distance.getValue(), p_orientation.getValue());
            p_lookAt.setValue(lookat);
        }
        else
        {
            serr << "Too many missing parameters ; taking default ..." << sendl;
        }
    }
    else
    {
        if(p_lookAt.isSet() && p_orientation.isSet())
        {
            //distance assumed to be set
            if(!p_distance.isSet())
                sout << "Missing distance parameter ; taking default value (0.0, 0.0, 0.0)" << sendl;

            Vec3 pos = getPositionFromOrientation(p_lookAt.getValue(), p_distance.getValue(), p_orientation.getValue());
            p_position.setValue(pos);
        }
        else
        {
            serr << "Too many missing parameters ; taking default ..." << sendl;
        }
    }

    currentLookAt = p_lookAt.getValue();
    currentDistance = p_distance.getValue();

}

void BaseCamera::reinit()
{
    //Data "LookAt" has changed
    //-> Orientation needs to be updated
    if(currentLookAt !=  p_lookAt.getValue())
    {
        Quat newOrientation = getOrientationFromLookAt(p_position.getValue(), p_lookAt.getValue());
        p_orientation.setValue(newOrientation);

        currentLookAt = p_lookAt.getValue();
    }
}

BaseCamera::Quat BaseCamera::getOrientationFromLookAt(const BaseCamera::Vec3 &pos, const BaseCamera::Vec3& lookat)
{
    Vec3 zAxis = -(lookat - pos);
    zAxis.normalize();

    Vec3 yAxis = cameraToWorldTransform(Vec3(0,1,0));

    Vec3 xAxis = yAxis.cross(zAxis) ;
    xAxis.normalize();

    //std::cout << xAxis.norm2() << std::endl;
    if (xAxis.norm2() < 0.00001)
        xAxis = cameraToWorldTransform(Vec3(1.0, 0.0, 0.0));
    xAxis.normalize();

    yAxis = zAxis.cross(xAxis);

    Quat q;
    q = q.createQuaterFromFrame(xAxis, yAxis, zAxis);
    q.normalize();
    return q;
}


BaseCamera::Vec3 BaseCamera::getLookAtFromOrientation(const BaseCamera::Vec3 &pos, const double &distance, const BaseCamera::Quat & orientation)
{
    Vec3 zWorld = orientation.rotate(Vec3(0,0,-1*distance));
    return zWorld+pos;
}

BaseCamera::Vec3 BaseCamera::getPositionFromOrientation(const BaseCamera::Vec3 &lookAt, const double &distance, const BaseCamera::Quat& orientation)
{
    Vec3 zWorld = orientation.rotate(Vec3(0,0,-1*distance));
    return zWorld-lookAt;
}

void BaseCamera::rotateCameraAroundPoint(Quat& rotation, const Vec3& point)
{
    Vec3 tempAxis;
    double tempAngle;
    Quat orientation = this->getOrientation();
    Vec3& position = *p_position.beginEdit();
    double distance = (point - p_position.getValue()).norm();

    rotation.quatToAxis(tempAxis, tempAngle);
    //std::cout << tempAxis << " " << tempAngle << std::endl;
    Quat tempQuat (orientation.inverse().rotate(-tempAxis ), tempAngle);
    orientation = orientation*tempQuat;

    Vec3 trans = point + orientation.rotate(Vec3(0,0,-distance)) - position;
    position = position + trans;

    p_orientation.setValue(orientation);
    p_position.endEdit();
}

void BaseCamera::rotateWorldAroundPoint(Quat& rotation, const Vec3&  point )
{
    Vec3 tempAxis;
    double tempAngle;
    Quat orientationCam = this->getOrientation();
    Vec3& positionCam = *p_position.beginEdit();

    rotation.quatToAxis(tempAxis, tempAngle);
    Quat tempQuat (orientationCam.rotate(-tempAxis), tempAngle);

    defaulttype::SolidTypes<double>::Transform world_H_cam(positionCam, orientationCam);
    defaulttype::SolidTypes<double>::Transform world_H_pivot(point, Quat());
    defaulttype::SolidTypes<double>::Transform pivotBefore_R_pivotAfter(Vec3(0.0,0.0,0.0), tempQuat);
    defaulttype::SolidTypes<double>::Transform camera_H_WorldAfter = world_H_cam.inversed() * world_H_pivot * pivotBefore_R_pivotAfter * world_H_pivot.inversed();
    //defaulttype::SolidTypes<double>::Transform camera_H_WorldAfter = worldBefore_H_cam.inversed()*worldBefore_R_worldAfter;

    positionCam = camera_H_WorldAfter.inversed().getOrigin();
    orientationCam = camera_H_WorldAfter.inversed().getOrientation();

    p_lookAt.setValue(getLookAtFromOrientation(positionCam, p_distance.getValue(), orientationCam));
    currentLookAt = p_lookAt.getValue();

    p_orientation.setValue(orientationCam);
    p_position.endEdit();
}

void BaseCamera::computeZ()
{
    //if (!p_zNear.isSet() || !p_zFar.isSet())
    {
        double zNear = 1e10;
        double zFar = -1e10;
        double zNearTemp = zNear;
        double zFarTemp = zFar;

        const Vec3& currentPosition = getPosition();
        Quat currentOrientation = this->getOrientation();

        const Vec3 & minBBox = p_minBBox.getValue();
        const Vec3 & maxBBox = p_maxBBox.getValue();

        currentOrientation.normalize();
        helper::gl::Transformation transform;

        currentOrientation.buildRotationMatrix(transform.rotation);
        for (unsigned int i=0 ; i< 3 ; i++)
            transform.translation[i] = -currentPosition[i];

        for (int corner=0; corner<8; ++corner)
        {
            Vec3 p((corner&1)?minBBox[0]:maxBBox[0],
                    (corner&2)?minBBox[1]:maxBBox[1],
                    (corner&4)?minBBox[2]:maxBBox[2]);
            //TODO: invert transform...
            p = transform * p;
            double z = -p[2];
            if (z < zNearTemp) zNearTemp = z;
            if (z > zFarTemp)  zFarTemp = z;
        }

        //get the same zFar and zNear calculations as QGLViewer
        sceneCenter = (minBBox + maxBBox)*0.5;

        double distanceCamToCenter = (currentPosition - sceneCenter).norm();
        double zClippingCoeff = 3.5;
        double zNearCoeff = 0.005;
        double sceneRadius = (fabs(zFarTemp-zNearTemp))*0.5;

        zFar = distanceCamToCenter + zClippingCoeff*sceneRadius ;
        zNear = distanceCamToCenter- zClippingCoeff*sceneRadius;

        double zMin = zNearCoeff * zClippingCoeff * sceneRadius;
        if (zNear < zMin)
            zNear = zMin;

        zNear = 0.1;
        zFar = 1000.0;

        p_zNear.setValue(zNear);
        p_zFar.setValue(zFar);

    }
}

} // namespace visualmodel

} //namespace component

} //namespace sofa

