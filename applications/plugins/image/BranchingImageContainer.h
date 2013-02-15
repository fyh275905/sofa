/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef IMAGE_BRANCHINGIMAGECONTAINER_H
#define IMAGE_BRANCHINGIMAGECONTAINER_H

#include "initImage.h"
#include "BranchingImage.h"

#include <sofa/core/visual/VisualParams.h>
#include <sofa/defaulttype/BoundingBox.h>


#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/defaulttype/Quat.h>


namespace sofa
{

namespace component
{

namespace container
{

using defaulttype::Vec;
using defaulttype::Vector3;
using defaulttype::Mat;

/**
   * \brief This component is responsible for loading images
   *
   *  ImageContainer scene options:
   *
   *  <b>template</b>
   *
   *  <b>filename</> - the name of the image file to be loaded. Currently supported filtypes:
   *
   */
template<class _ImageTypes>
class BranchingImageContainer : public virtual core::objectmodel::BaseObject
{
public:
    typedef core::objectmodel::BaseObject Inherited;
    SOFA_CLASS( SOFA_TEMPLATE(BranchingImageContainer, _ImageTypes),Inherited);

    // image data
    typedef _ImageTypes ImageTypes;
    typedef typename ImageTypes::T T;
    typedef typename ImageTypes::Dimension Dimension;
    typedef helper::WriteAccessor<Data< ImageTypes > > waImage;
    typedef helper::ReadAccessor<Data< ImageTypes > > raImage;
    Data< ImageTypes > branchingImage;

    // transform data
    typedef SReal Real;
    typedef defaulttype::ImageLPTransform<Real> TransformType;
    typedef helper::WriteAccessor<Data< TransformType > > waTransform;
    typedef helper::ReadAccessor<Data< TransformType > > raTransform;
    Data< TransformType > transform;

    Data<bool> drawBB;

    // input file
    sofa::core::objectmodel::DataFileName m_filename;

    virtual std::string getTemplateName() const	{ return templateName(this); }
    static std::string templateName(const BranchingImageContainer<ImageTypes>* = NULL) {	return ImageTypes::Name(); }

    BranchingImageContainer() : Inherited()
        , branchingImage(initData(&branchingImage,ImageTypes(),"branchingImage","BranchingImage"))
        , transform(initData(&transform, TransformType(), "transform" , "Transform"))
        , drawBB(initData(&drawBB,true,"drawBB","draw bounding box"))
        , m_filename(initData(&m_filename,"filename","BranchingImage file"))
    {
        this->addAlias(&branchingImage, "inputBranchingImage");
        this->addAlias(&transform, "inputTransform");
        transform.setGroup("Transform");
    }


    virtual void clear()
    {
        waImage wimage(this->branchingImage);
        wimage->clear();
    }

    virtual ~BranchingImageContainer() {clear();}

    virtual void init()
    {
        if( !branchingImage.getValue().dimension[ImageTypes::DIMENSION_T] && !load() )
            serr << "no input image "<<sendl;
    }




protected:


    bool load()
    {
        if (!this->m_filename.isSet()) return false;

        std::string fname( this->m_filename.getFullPath() );
        if( !sofa::helper::system::DataRepository.findFile( fname ) )
        {
            serr << "cannot find "<<fname<<sendl;
            return false;
        }
        fname = sofa::helper::system::DataRepository.getFile( fname );

        if( fname.find(".mhd")!=std::string::npos || fname.find(".MHD")!=std::string::npos || fname.find(".Mhd")!=std::string::npos
              || fname.find(".bia")!=std::string::npos || fname.find(".BIA")!=std::string::npos || fname.find(".Bia")!=std::string::npos)
        {
            if(fname.find(".bia")!=std::string::npos || fname.find(".BIA")!=std::string::npos || fname.find(".Bia")!=std::string::npos)      fname.replace(fname.find_last_of('.')+1,fname.size(),"mhd");

            double scale[3]={1.,1.,1.},translation[3]={0.,0.,0.},affine[9]={1.,0.,0.,0.,1.,0.,0.,0.,1.},offsetT=0.,scaleT=1.;
            bool isPerspective=false;

            if( waImage( branchingImage )->load( fname.c_str(), scale, translation, affine, &offsetT, &scaleT, &isPerspective ) )
            {
                waTransform wtransform( transform );

                for(unsigned int i=0;i<3;i++) wtransform->getScale()[i]=(Real)scale[i];
                for(unsigned int i=0;i<3;i++) wtransform->getTranslation()[i]=(Real)translation[i];
                Mat<3,3,Real> R; for(unsigned int i=0;i<3;i++) for(unsigned int j=0;j<3;j++) R[i][j]=(Real)affine[3*i+j];
                helper::Quater< Real > q; q.fromMatrix(R);
                // wtransform->getRotation()=q.toEulerVector() * (Real)180.0 / (Real)M_PI ;  //  this does not convert quaternion to euler angles
                if(q[0]*q[0]+q[1]*q[1]==0.5 || q[1]*q[1]+q[2]*q[2]==0.5) {q[3]+=10-3; q.normalize();} // hack to avoid singularities
                if (!this->transform.isSet()) {
                    wtransform->getRotation()[0]=atan2(2*(q[3]*q[0]+q[1]*q[2]),1-2*(q[0]*q[0]+q[1]*q[1])) * (Real)180.0 / (Real)M_PI;
                    wtransform->getRotation()[1]=asin(2*(q[3]*q[1]-q[2]*q[0])) * (Real)180.0 / (Real)M_PI;
                    wtransform->getRotation()[2]=atan2(2*(q[3]*q[2]+q[0]*q[1]),1-2*(q[1]*q[1]+q[2]*q[2])) * (Real)180.0 / (Real)M_PI;
                    wtransform->getOffsetT()=(Real)offsetT;
                    wtransform->getScaleT()=(Real)scaleT;
                    wtransform->isPerspective()=isPerspective;
                }
                return true;
            }
        }

        return false;
    }




    void getCorners(Vec<8,Vector3> &c) // get image corners
    {
        raImage rimage(this->branchingImage);
        const Dimension dim = rimage->getDimension();

        Vec<8,Vector3> p;
        p[0]=Vector3(-0.5,-0.5,-0.5);
        p[1]=Vector3(dim[0]-0.5,-0.5,-0.5);
        p[2]=Vector3(-0.5,dim[1]-0.5,-0.5);
        p[3]=Vector3(dim[0]-0.5,dim[1]-0.5,-0.5);
        p[4]=Vector3(-0.5,-0.5,dim[2]-0.5);
        p[5]=Vector3(dim[0]-0.5,-0.5,dim[2]-0.5);
        p[6]=Vector3(-0.5,dim[1]-0.5,dim[2]-0.5);
        p[7]=Vector3(dim[0]-0.5,dim[1]-0.5,dim[2]-0.5);

        raTransform rtransform(this->transform);
        for(unsigned int i=0; i<p.size(); i++) c[i]=rtransform->fromImage(p[i]);
    }

    virtual void computeBBox(const core::ExecParams*  params )
    {
        if (!drawBB.getValue()) return;
        Vec<8,Vector3> c;
        getCorners(c);

        Real bbmin[3]  = {c[0][0],c[0][1],c[0][2]} , bbmax[3]  = {c[0][0],c[0][1],c[0][2]};
        for(unsigned int i=1; i<c.size(); i++)
            for(unsigned int j=0; j<3; j++)
            {
                if(bbmin[j]>c[i][j]) bbmin[j]=c[i][j];
                if(bbmax[j]<c[i][j]) bbmax[j]=c[i][j];
            }
        this->f_bbox.setValue(params,sofa::defaulttype::TBoundingBox<Real>(bbmin,bbmax));
    }

    void draw(const core::visual::VisualParams* vparams)
    {
        // draw bounding box

        if (!vparams->displayFlags().getShowVisualModels()) return;
        if (!drawBB.getValue()) return;

        glPushAttrib( GL_LIGHTING_BIT || GL_ENABLE_BIT || GL_LINE_BIT );
        glPushMatrix();

        const float color[]= {1.,0.5,0.5,0.}, specular[]= {0.,0.,0.,0.};
        glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0.0);
        glColor4fv(color);
        glLineWidth(2.0);

        Vec<8,Vector3> c;
        getCorners(c);

        glBegin(GL_LINE_LOOP);	glVertex3d(c[0][0],c[0][1],c[0][2]); glVertex3d(c[1][0],c[1][1],c[1][2]); glVertex3d(c[3][0],c[3][1],c[3][2]); glVertex3d(c[2][0],c[2][1],c[2][2]);	glEnd ();
        glBegin(GL_LINE_LOOP);  glVertex3d(c[0][0],c[0][1],c[0][2]); glVertex3d(c[4][0],c[4][1],c[4][2]); glVertex3d(c[6][0],c[6][1],c[6][2]); glVertex3d(c[2][0],c[2][1],c[2][2]);	glEnd ();
        glBegin(GL_LINE_LOOP);	glVertex3d(c[0][0],c[0][1],c[0][2]); glVertex3d(c[1][0],c[1][1],c[1][2]); glVertex3d(c[5][0],c[5][1],c[5][2]); glVertex3d(c[4][0],c[4][1],c[4][2]);	glEnd ();
        glBegin(GL_LINE_LOOP);	glVertex3d(c[1][0],c[1][1],c[1][2]); glVertex3d(c[3][0],c[3][1],c[3][2]); glVertex3d(c[7][0],c[7][1],c[7][2]); glVertex3d(c[5][0],c[5][1],c[5][2]);	glEnd ();
        glBegin(GL_LINE_LOOP);	glVertex3d(c[7][0],c[7][1],c[7][2]); glVertex3d(c[5][0],c[5][1],c[5][2]); glVertex3d(c[4][0],c[4][1],c[4][2]); glVertex3d(c[6][0],c[6][1],c[6][2]);	glEnd ();
        glBegin(GL_LINE_LOOP);	glVertex3d(c[2][0],c[2][1],c[2][2]); glVertex3d(c[3][0],c[3][1],c[3][2]); glVertex3d(c[7][0],c[7][1],c[7][2]); glVertex3d(c[6][0],c[6][1],c[6][2]);	glEnd ();

        glPopMatrix ();
        glPopAttrib();
    }


};






}

}

}


#endif // IMAGE_BRANCHINGIMAGECONTAINER_H
