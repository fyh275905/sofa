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
#ifndef SOFA_IMAGE_BRANCHINGIMAGECONVERTER_H
#define SOFA_IMAGE_BRANCHINGIMAGECONVERTER_H

#include "initImage.h"
#include "ImageTypes.h"
#include "BranchingImage.h"
#include <sofa/core/DataEngine.h>


namespace sofa
{

namespace component
{

namespace engine
{



/// convert a flat image to a branching image with an optional coarsening
/// no coarsening (coarseningLevels==0) returns a branching image without any ramification
/// only coarsening can create ramifications
/// the coarse size will be equal to inputsize/2^coarseningLevels (each level subdivides the size x/y/z by 2)
/// This implementation improves the algorithm presented in: Nesme, Kry, Jeřábková, Faure, "Preserving Topology and Elasticity for Embedded Deformable Models", Siggraph09
/// because the coarse configuration is directly computed from the fine resolution.
template <class Tin,class Tout=Tin>
class ImageToBranchingImageConverter : public core::DataEngine
{
public:
    typedef core::DataEngine Inherited;
    SOFA_CLASS(SOFA_TEMPLATE2(ImageToBranchingImageConverter,Tin,Tout),Inherited);


    typedef SReal Real;

    typedef defaulttype::Image<Tin> ImageTypes;
    typedef defaulttype::BranchingImage<Tout> BranchingImageTypes;
    typedef defaulttype::ImageLPTransform<Real> TransformType;
    typedef defaulttype::Image<unsigned long> ImageLabelTypes; ///< a component labeling image. The labeling indices are divided in independant blocks, each block corresponding to a coarse voxel. A label index gives the offset in the list of the superimposed voxel at coarse resolution that contains the fine pixel.


    Data<ImageTypes> inputImage;
    Data<BranchingImageTypes> outputBranchingImage;
    Data<ImageLabelTypes> outputMappingImage;
    Data<bool> createMappingImage;
    Data<unsigned> coarseningLevels;
    Data<unsigned> superimpositionType;

    Data<TransformType> inputTransform, outputTransform;


    virtual std::string getTemplateName() const { return templateName(this); }
    static std::string templateName(const ImageToBranchingImageConverter<Tin,Tout>* = NULL) { return ImageTypes::Name()+std::string(",")+BranchingImageTypes::Name(); }

    ImageToBranchingImageConverter()    :   Inherited()
        , inputImage(initData(&inputImage,ImageTypes(),"inputImage","Image to coarsen"))
        , outputBranchingImage(initData(&outputBranchingImage,BranchingImageTypes(),"outputBranchingImage","Coarsened BranchingImage"))
        , outputMappingImage(initData(&outputMappingImage,ImageLabelTypes(),"outputMappingImage","A regular with the same size as inputImage, its associated transform is inputTransform. Each pixel stores the offset of its containing SuperimposedVoxel"))
        , createMappingImage(initData(&createMappingImage,true,"createMappingImage","Will outputMappingImage be created?"))
        , coarseningLevels(initData(&coarseningLevels,(unsigned)0,"coarseningLevels","How many coarsenings (subdividing x/y/z by 2)?"))
        , superimpositionType(initData(&superimpositionType,(unsigned)0,"superimpositionType","Which value for superimposed voxels? (0->sum (default), 1->average, 2->ratio, 3->count)"))
        , inputTransform(initData(&inputTransform,TransformType(),"inputTransform","Input Transform"))
        , outputTransform(initData(&outputTransform,TransformType(),"outputTransform","Output Transform"))
    {
        inputImage.setReadOnly(true);
        outputBranchingImage.setReadOnly(true);
        this->addAlias(&inputImage, "image");
        this->addAlias(&outputBranchingImage, "branchingImage");
        this->addAlias(&outputTransform, "transform");
    }

    virtual ~ImageToBranchingImageConverter()
    {
    }

    virtual void init()
    {
        addInput(&inputImage);
        addInput(&coarseningLevels);
        addInput(&superimpositionType);
        addInput(&inputTransform);
        addInput(&createMappingImage);
        addOutput(&outputBranchingImage);
        addOutput(&outputMappingImage);
        addOutput(&outputTransform);
        setDirtyValue();
    }

    virtual void reinit()
    {
        update();
    }


protected:

    virtual void update()
    {
        cleanDirty();

        BranchingImageTypes &output = *outputBranchingImage.beginEdit();
        ImageLabelTypes &labelImages = *outputMappingImage.beginEdit();
        const ImageTypes &input = inputImage.getValue();
        TransformType& transform = *outputTransform.beginEdit();

        transform = inputTransform.getValue();

        typename ImageTypes::imCoord labelDimension = input.getDimensions();
        labelDimension[BranchingImageTypes::DIMENSION_S] = 1;
        labelImages.setDimensions( labelDimension );

        if( coarseningLevels.getValue()<1 )
        {
            output = input; // operator= is performing the conversion without modifying the resolution, so with flat neighbourhood
            if( createMappingImage.getValue() )
                for( unsigned t=0 ; t<labelDimension[BranchingImageTypes::DIMENSION_T] ; ++t )
                    labelImages.getCImg(t).fill( 0 );
        }
        else
        {
            unsigned nb = pow( (float)2, (int)coarseningLevels.getValue() ); // nb fine voxels in a coarse voxel in each direction x/y/z

            const typename ImageTypes::imCoord inputDimension = input.getDimensions();
            typename BranchingImageTypes::Dimension outputDimension = inputDimension;
            outputDimension[BranchingImageTypes::DIMENSION_X] = ceil( outputDimension[BranchingImageTypes::DIMENSION_X]/(float)nb );
            outputDimension[BranchingImageTypes::DIMENSION_Y] = ceil( outputDimension[BranchingImageTypes::DIMENSION_Y]/(float)nb );
            outputDimension[BranchingImageTypes::DIMENSION_Z] = ceil( outputDimension[BranchingImageTypes::DIMENSION_Z]/(float)nb );

            output.setDimension( outputDimension );

            // COARSE TRANSFORM
            transform.getScale() *= nb;
            // transform translation is given from the center of the pixel (0,0,0), but we want the corners (0,0,0) to be at the same spatial position
            transform.getTranslation() += ( transform.getScale() - inputTransform.getValue().getScale() ) / 2.0;

            for( unsigned t=0 ; t<outputDimension[BranchingImageTypes::DIMENSION_T] ; ++t )
            {
                const typename ImageTypes::CImgT& input_t = input.getCImg(t);
                typename BranchingImageTypes::BranchingImage3D& output_t = output.imgList[t];

                // a global labeling image, where labels have sens only in small blocks corresponding to the coarse voxels
                typename ImageLabelTypes::CImgT& labelImage = labelImages.getCImg(t);

                // FIND SUPERIMPOSED VOXELS

                unsigned index1d = 0;
                for( unsigned z=0 ; z<outputDimension[BranchingImageTypes::DIMENSION_Z] ; ++z )
                for( unsigned y=0 ; y<outputDimension[BranchingImageTypes::DIMENSION_Y] ; ++y )
                for( unsigned x=0 ; x<outputDimension[BranchingImageTypes::DIMENSION_X] ; ++x )
                {
                    unsigned finex = x*nb, finexend = std::min( (x+1)*nb, inputDimension[BranchingImageTypes::DIMENSION_X] ) - 1;
                    unsigned finey = y*nb, fineyend = std::min( (y+1)*nb, inputDimension[BranchingImageTypes::DIMENSION_Y] ) - 1;
                    unsigned finez = z*nb, finezend = std::min( (z+1)*nb, inputDimension[BranchingImageTypes::DIMENSION_Z] ) - 1;

                    // the sub-image storing all fine voxels included in the coarse voxel
                    typename ImageTypes::CImgT subImage = input_t.get_crop( finex, finey, finez, finexend, fineyend, finezend );

                    // convert the subimage to a bool image with only 1 canal
                    cimg_library::CImg<bool> subBinaryImage = subImage.get_resize( subImage.width(), subImage.height(), subImage.depth(), 1, 3 );


////// @todo   improve this part, by implementing a CImg::get_label function that only consider not empty pixels for labelling (and all empty pixels have label=0)

                    // connected component labeling
                    cimg_library::CImg<unsigned long> subLabelImage = subBinaryImage.get_label( false );

                    unsigned nbLabels = subLabelImage.max();


                    if( !nbLabels )
                    {
                        if( subBinaryImage(0,0,0) ) // when all voxels are filled, there is only one label==0, replace it by 1
                        {
                            nbLabels=1;
                            subLabelImage.fill(1);
                        }
                        else // all empty
                        {
                            nbLabels=0;
                        }
                    }
                    else // some are empty, some are not -> enforce the label 0 to empty voxels
                    {
                        unsigned label0 = 0;
                        cimg_foroff( subLabelImage, off ) if( !subBinaryImage(off) ) { label0=subLabelImage(off); break; }
                        cimg_foroff( subLabelImage, off ) if( !subBinaryImage(off) ) subLabelImage(off)=0; else if( !subLabelImage(off) ) subLabelImage(off)=label0;

                        // give a continue index from 0 to max (without hole)
                        // hole could have been created by several void independant components
                        std::map<unsigned,unsigned> continueMap;
                        continueMap[0] = 0;  // enforce 0 to stay 0
                        unsigned continueIndex = 1;

                        cimg_foroff( subLabelImage, off ) if( continueMap.find(subLabelImage(off))==continueMap.end() ) continueMap[subLabelImage(off)]=continueIndex++;
                        cimg_foroff( subLabelImage, off ) subLabelImage(off) = continueMap[subLabelImage(off)];
                        nbLabels = continueIndex-1;
                    }

///// end improve

                    if( nbLabels )
                    {

                        // a superimposed voxel per independant component
                        output_t[index1d].resize( nbLabels, outputDimension[BranchingImageTypes::DIMENSION_S] );


                        // compute the superimposed values depending on superimpositionType
                        switch( superimpositionType.getValue() )
                        {
                            case 1: // average
                            {
                                for( unsigned v=0 ; v<output_t[index1d].size() ; ++v )
                                {
                                    typename BranchingImageTypes::ConnectionVoxel& voutput = output_t[index1d][v];
                                    voutput.resize( outputDimension[BranchingImageTypes::DIMENSION_S] );
                                    // count nb pixels==v in subLabelImage
                                    int nbLabelsV = 0;
                                    cimg_forXYZ( subLabelImage, subx,suby,subz )
                                        if( subLabelImage(subx,suby,subz) == v+1 )
                                        {
                                            nbLabelsV++;
                                            for( unsigned s=0 ; s<outputDimension[BranchingImageTypes::DIMENSION_S] ; ++s )
                                            {
                                                assert( typeid(Tout)==typeid(bool) ||voutput[s] <= std::numeric_limits<Tout>::max()-subImage(subx,suby,subz,s) );
                                                voutput[s] += subImage(subx,suby,subz,s);
                                            }
                                        }
                                    for( unsigned s=0 ; s<outputDimension[BranchingImageTypes::DIMENSION_S] ; ++s ) voutput[s] /= nbLabelsV;
                                }
                                break;
                            }
                            case 2: // ratio (nb fine in superimposed voxel / total fine)
                            {
                                for( unsigned v=0 ; v<output_t[index1d].size() ; ++v )
                                {
                                    typename BranchingImageTypes::ConnectionVoxel& voutput = output_t[index1d][v];
                                    voutput.resize( outputDimension[BranchingImageTypes::DIMENSION_S] );
                                    // count nb pixels==v in subLabelImage
                                    int nbLabelsV = 0;
                                    cimg_foroff( subLabelImage, off )
                                        if( subLabelImage(off) == v+1 )
                                        {
                                            nbLabelsV++;
                                        }
                                    for( unsigned s=0 ; s<outputDimension[BranchingImageTypes::DIMENSION_S] ; ++s )
                                    {
                                        voutput[s] = 1.0/(float)nbLabelsV;
                                    }
                                }
                                break;
                            }
                            case 3: // count (nb fine in superimposed voxel)
                            {
                                for( unsigned v=0 ; v<output_t[index1d].size() ; ++v )
                                {
                                    typename BranchingImageTypes::ConnectionVoxel& voutput = output_t[index1d][v];
                                    voutput.resize( outputDimension[BranchingImageTypes::DIMENSION_S] );
                                    // count nb pixels==v in subLabelImage
                                    int nbLabelsV = 0;
                                    cimg_foroff( subLabelImage, off ) if( subLabelImage(off) == v+1 ) nbLabelsV++;
                                    for( unsigned s=0 ; s<outputDimension[BranchingImageTypes::DIMENSION_S] ; ++s )
                                    {
    //                                    assert( voutput[s] <= std::numeric_limits<Tout>::max()-nbLabelsV );
                                        voutput[s] = nbLabelsV;
                                    }
                                }
                                break;
                            }
                            case 0: // sum
                            default:
                            {
                                for( unsigned v=0 ; v<output_t[index1d].size() ; ++v )
                                {
                                    typename BranchingImageTypes::ConnectionVoxel& voutput = output_t[index1d][v];
                                    voutput.resize( outputDimension[BranchingImageTypes::DIMENSION_S] );
                                    cimg_forXYZ( subLabelImage, subx,suby,subz )
                                        if( subLabelImage(subx,suby,subz) == v+1 )
                                        {
                                            for( unsigned s=0 ; s<outputDimension[BranchingImageTypes::DIMENSION_S] ; ++s )
                                            {
                                                assert( typeid(Tout)==typeid(bool) || voutput[s] <= std::numeric_limits<Tout>::max()-subImage(subx,suby,subz,s) );
                                                voutput[s] += subImage(subx,suby,subz,s);
                                            }
                                        }
                                }
                                break;
                            }
                        }
                    }

                    // copy the block (corresponding to the coarse voxel) in the global label image
                    cimg_library::copySubImage( labelImage, subLabelImage, finex, finey, finez );

                    ++index1d;
                }


                // FIND CONNECTIVITY

                index1d = 0;
                for( unsigned z=0 ; z<outputDimension[BranchingImageTypes::DIMENSION_Z] ; ++z )
                for( unsigned y=0 ; y<outputDimension[BranchingImageTypes::DIMENSION_Y] ; ++y )
                for( unsigned x=0 ; x<outputDimension[BranchingImageTypes::DIMENSION_X] ; ++x )
                {
                    unsigned finex = x*nb, finexend = std::min( (x+1)*nb, inputDimension[BranchingImageTypes::DIMENSION_X] ) - 1;
                    unsigned finey = y*nb, fineyend = std::min( (y+1)*nb, inputDimension[BranchingImageTypes::DIMENSION_Y] ) - 1;
                    unsigned finez = z*nb, finezend = std::min( (z+1)*nb, inputDimension[BranchingImageTypes::DIMENSION_Z] ) - 1;

                    // for all fine voxels included in the coarse voxel
                    for( unsigned fz=finez ; fz<=finezend ; ++fz )
                    for( unsigned fy=finey ; fy<=fineyend ; ++fy )
                    for( unsigned fx=finex ; fx<=finexend ; ++fx )
                    {
                        unsigned coarseOffset = labelImage(fx,fy,fz); // position of coarse voxel including the fine voxel(fx,fy,fz) in the superimposed voxels

                        if( coarseOffset ) // not empty
                        {
                            coarseOffset -= 1; // the label 1 go to the offset/position 0 in the superimposed vector

                            // look at all fine neighbours of the fine voxel
                            if( fx>0 )
                            {
                                 unsigned neighbourCoarseOffset = labelImage(fx-1,fy,fz);
                                 if( neighbourCoarseOffset ) // neighbour is not empty
                                 {
                                     neighbourCoarseOffset -= 1;
                                     if( fx-1 < finex ) // both are not in the same coarse voxel
                                     {
                                         const unsigned neighbourCoarseIndex = index1d-1;
                                        // connect index grossier de vf et de n
                                         output_t[index1d][coarseOffset].addNeighbour( typename BranchingImageTypes::VoxelIndex( neighbourCoarseIndex, neighbourCoarseOffset ) );
                                         output_t[neighbourCoarseIndex][neighbourCoarseOffset].addNeighbour( typename BranchingImageTypes::VoxelIndex( index1d, coarseOffset ) );
                                     }
                                 }
                            }
                            if( fy>0 )
                            {
                                 unsigned neighbourCoarseOffset = labelImage(fx,fy-1,fz);
                                 if( neighbourCoarseOffset ) // neighbour is not empty
                                 {
                                     neighbourCoarseOffset -= 1;
                                     if( fy-1 < finey ) // both are not in the same coarse voxel
                                     {
                                         const unsigned neighbourCoarseIndex = index1d-outputDimension[BranchingImageTypes::DIMENSION_X];
                                        // connect index grossier de vf et de n
                                         output_t[index1d][coarseOffset].addNeighbour( typename BranchingImageTypes::VoxelIndex( neighbourCoarseIndex, neighbourCoarseOffset ) );
                                         output_t[neighbourCoarseIndex][neighbourCoarseOffset].addNeighbour( typename BranchingImageTypes::VoxelIndex( index1d, coarseOffset ) );
                                     }
                                 }
                            }
                            if( fz>0 )
                            {
                                 unsigned neighbourCoarseOffset = labelImage(fx,fy,fz-1);
                                 if( neighbourCoarseOffset ) // neighbour is not empty
                                 {
                                     neighbourCoarseOffset -= 1;
                                     if( fz-1 < finez ) // both are not in the same coarse voxel
                                     {
                                         const unsigned neighbourCoarseIndex = index1d-output.sliceSize;
                                        // connect index grossier de vf et de n
                                         output_t[index1d][coarseOffset].addNeighbour( typename BranchingImageTypes::VoxelIndex( neighbourCoarseIndex, neighbourCoarseOffset ) );
                                         output_t[neighbourCoarseIndex][neighbourCoarseOffset].addNeighbour( typename BranchingImageTypes::VoxelIndex( index1d, coarseOffset ) );
                                     }
                                 }
                            }
                        }
                    }
                    ++index1d;
                }
            } // for t
        }

        // delete the label image if no longer wanted
        if( !createMappingImage.getValue() ) labelImages.clear();

        outputTransform.endEdit();
        outputMappingImage.endEdit();
        outputBranchingImage.endEdit();

        //assert( !outputBranchingImage.getValue().isEqual(inputImage.getValue()) );


        if( f_printLog.getValue() )
        {
            std::cerr<<"ImageToBranchingImageConverter::update - conversion finished ";
            std::cerr<<"("<<inputImage.getValue().approximativeSizeInBytes()<<" Bytes -> "<<outputBranchingImage.getValue().approximativeSizeInBytes()<<" Bytes -> x"<<outputBranchingImage.getValue().approximativeSizeInBytes()/(float)inputImage.getValue().approximativeSizeInBytes()<<")\n";
        }
    }

};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/// convert a branching image to a flat image
template <class Tin,class Tout=Tin>
class BranchingImageToImageConverter : public core::DataEngine
{
public:
    typedef core::DataEngine Inherited;
    SOFA_CLASS(SOFA_TEMPLATE2(BranchingImageToImageConverter,Tin,Tout),Inherited);

    typedef defaulttype::Image<Tout> ImageTypes;
    typedef defaulttype::BranchingImage<Tin> BranchingImageTypes;

    Data<ImageTypes> image;
    Data<BranchingImageTypes> inputBranchingImage;
    Data<unsigned> conversionType;

    virtual std::string getTemplateName() const    { return templateName(this);    }
    static std::string templateName(const BranchingImageToImageConverter<Tin,Tout>* = NULL) { return BranchingImageTypes::Name()+std::string(",")+ImageTypes::Name(); }

    BranchingImageToImageConverter()    :   Inherited()
        , image(initData(&image,ImageTypes(),"image","output Image"))
        , inputBranchingImage(initData(&inputBranchingImage,BranchingImageTypes(),"inputBranchingImage","input BranchingImage"))
        , conversionType(initData(&conversionType,(unsigned)2,"conversionType","0->first voxel, 1->mean, 2->nb superimposed voxels (default), 3->sum"))
    {
        inputBranchingImage.setReadOnly(true);
        this->addAlias(&inputBranchingImage, "branchingImage");
        this->addAlias(&image, "outputImage");
    }

    virtual ~BranchingImageToImageConverter()
    {
    }

    virtual void init()
    {
        addInput(&inputBranchingImage);
        addInput(&conversionType);
        addOutput(&image);
        setDirtyValue();
    }

    virtual void reinit()
    {
        update();
    }


protected:

    virtual void update()
    {
        cleanDirty();

        ImageTypes &img = *image.beginEdit();
        inputBranchingImage.getValue().toImage( img, conversionType.getValue() );

        if( f_printLog.getValue() ) std::cerr<<"BranchingImageToImageEngine::update - conversion finished ("<<inputBranchingImage.getValue().approximativeSizeInBytes()<<" Bytes -> "<<img.approximativeSizeInBytes()<<" Bytes -> x"<<img.approximativeSizeInBytes()/(float)inputBranchingImage.getValue().approximativeSizeInBytes()<<")\n";

        image.endEdit();
    }

};


} // namespace engine

} // namespace component

} // namespace sofa

#endif // SOFA_IMAGE_BRANCHINGIMAGECONVERTER_H
