#pragma once

#include <SofaSphFluid/config.h>

#include <sofa/core/visual/VisualModel.h>
#include <sofa/gl/FrameBufferObject.h>
#include <sofa/gl/GLSLShader.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/helper/types/RGBAColor.h>

namespace sofa
{
namespace component
{
namespace visualmodel
{
using namespace sofa::defaulttype;
/**
 *  \brief Render volume using particles
 *
 */

// http://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf

template<class DataTypes>
class SOFA_SPH_FLUID_API OglFluidModel : public core::visual::VisualModel
{
public:
    SOFA_CLASS(OglFluidModel, core::visual::VisualModel);
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Real Real;

private:
    Data< VecCoord > m_positions;
	VecCoord m_previousPositions;

    GLuint m_posVBO;
    sofa::gl::FrameBufferObject* m_spriteDepthFBO;
    sofa::gl::FrameBufferObject* m_spriteThicknessFBO;
    sofa::gl::FrameBufferObject* m_spriteNormalFBO;
    sofa::gl::FrameBufferObject* m_spriteBlurDepthHFBO;
    sofa::gl::FrameBufferObject* m_spriteBlurDepthVFBO;
    sofa::gl::FrameBufferObject* m_spriteBlurThicknessHFBO;
    sofa::gl::FrameBufferObject* m_spriteBlurThicknessVFBO;
    sofa::gl::FrameBufferObject* m_spriteShadeFBO;

    sofa::gl::GLSLShader m_spriteShader;
    sofa::gl::GLSLShader m_spriteNormalShader;
    sofa::gl::GLSLShader m_spriteBlurDepthShader;
    sofa::gl::GLSLShader m_spriteBlurThicknessShader;
    sofa::gl::GLSLShader m_spriteShadeShader;
    sofa::gl::GLSLShader m_spriteFinalPassShader;

    void drawSprites(const core::visual::VisualParams* vparams);
    void updateVertexBuffer();
protected:
    OglFluidModel();
    virtual ~OglFluidModel();
public:
    Data<unsigned int> d_debugFBO;
    Data<float> d_spriteRadius;
    Data<float> d_spriteThickness;
    Data<float> d_spriteBlurRadius;
    Data<float> d_spriteBlurScale;
    Data<float> d_spriteBlurDepthFalloff;
    Data<sofa::helper::types::RGBAColor> d_spriteDiffuseColor;


    void init() override;
    void initVisual() override;
    void fwdDraw(core::visual::VisualParams*) override;
    void bwdDraw(core::visual::VisualParams*) override;
    void drawVisual(const core::visual::VisualParams* vparams) override;
    void drawTransparent(const core::visual::VisualParams* vparams) override;
    void computeBBox(const core::ExecParams* params, bool onlyVisible = false) override;

    virtual void updateVisual() override;

    static std::string templateName(const OglFluidModel<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

    virtual std::string getTemplateName() const override
    {
        return templateName(this);
    }

};

}

}

}

