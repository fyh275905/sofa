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
#pragma once

#include <sofa/helper/config.h>
#include <sofa/helper/types/RGBAColor.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/fwd.h>
#include <sofa/defaulttype/Quat.h>
#include <vector>

namespace sofa::helper::visual
{

/**
 *  \brief Utility class to perform debug drawing.
 *
 *  Class which contains a set of methods to perform minimal debug drawing regardless of the graphics API used.
 *  Components receive a pointer to the DrawTool through the VisualParams parameter of their draw method.
 *  Sofa provides a default concrete implementation of this class for the OpenGL API with the DrawToolGL class.
 *k
 */

class DrawTool
{

public:
    typedef sofa::helper::types::RGBAColor RGBAColor;
    typedef sofa::defaulttype::Vec3f   Vec3f;
    typedef sofa::defaulttype::Vector3 Vector3;
    typedef sofa::defaulttype::Vec<3,int> Vec3i;
    typedef sofa::defaulttype::Vec<2,int> Vec2i;
    typedef sofa::defaulttype::Quaternion Quaternion;

    DrawTool() { clear(); }
    virtual ~DrawTool() {}

    virtual void init() = 0;

    /// @name Primitive rendering methods
    /// @{
    virtual void drawPoints(const std::vector<Vector3> &points, float size,  const  RGBAColor& colour) = 0 ;
    virtual void drawPoints(const std::vector<Vector3> &points, float size, const std::vector<RGBAColor>& colour) = 0;

    virtual void drawLine(const Vector3 &p1, const Vector3 &p2, const RGBAColor& colour) =  0;
    virtual void drawInfiniteLine(const Vector3 &point, const Vector3 &direction, const RGBAColor& color) = 0;
    virtual void drawLines(const std::vector<Vector3> &points, float size, const RGBAColor& colour) = 0 ;
    virtual void drawLines(const std::vector<Vector3> &points, float size, const std::vector<RGBAColor>& colours) = 0 ;
    virtual void drawLines(const std::vector<Vector3> &points, const std::vector< Vec2i > &index , float size, const RGBAColor& colour) = 0 ;

    virtual void drawLineStrip(const std::vector<Vector3> &points, float size, const RGBAColor& colour) = 0 ;
    virtual void drawLineLoop(const std::vector<Vector3> &points, float size, const RGBAColor& colour) = 0 ;

    virtual void drawDisk(float radius, double from, double to, int resolution, const RGBAColor& color) = 0;
    virtual void drawCircle(float radius, float lineThickness, int resolution, const RGBAColor& color) = 0;

    virtual void drawTriangles(const std::vector<Vector3> &points, const RGBAColor& colour) = 0 ;
    virtual void drawTriangles(const std::vector<Vector3> &points, const Vector3& normal, const RGBAColor& colour) = 0 ;
    virtual void drawTriangles(const std::vector<Vector3> &points,
            const std::vector< Vec3i > &index,
            const std::vector<Vector3>  &normal,
            const RGBAColor& colour) = 0 ;
    virtual void drawTriangles(const std::vector<Vector3> &points,
        const std::vector< Vec3i > &index,
        const std::vector<Vector3>  &normal,
        const std::vector<RGBAColor>& colour) = 0;
    virtual void drawTriangles(const std::vector<Vector3> &points,
            const std::vector< RGBAColor > &colour) = 0 ;
    virtual void drawTriangles(const std::vector<Vector3> &points,
            const std::vector<Vector3>  &normal,
            const std::vector< RGBAColor > &colour) = 0 ;
    virtual void drawTriangleStrip(const std::vector<Vector3> &points,
            const std::vector<Vector3>  &normal,
            const RGBAColor& colour) = 0 ;
    virtual void drawTriangleFan(const std::vector<Vector3> &points,
            const std::vector<Vector3>  &normal,
            const RGBAColor& colour) = 0 ;



    virtual void drawFrame   (const Vector3& position, const Quaternion &orientation, const Vec3f &size) = 0 ;
    virtual void drawFrame   (const Vector3& position, const Quaternion &orientation, const Vec3f &size, const RGBAColor &colour) = 0 ;

    virtual void drawSpheres (const std::vector<Vector3> &points, const std::vector<float>& radius, const RGBAColor& colour) = 0;
    virtual void drawSpheres (const std::vector<Vector3> &points, float radius, const RGBAColor& colour) = 0 ;
    virtual void drawFakeSpheres(const std::vector<Vector3> &points, const std::vector<float>& radius, const RGBAColor& colour) = 0;
    virtual void drawFakeSpheres(const std::vector<Vector3> &points, float radius, const RGBAColor& colour) = 0;

    virtual void drawCone    (const Vector3& p1, const Vector3 &p2, float radius1, float radius2, const RGBAColor& colour, int subd=16) = 0 ;

    /// Draw a cube of size one centered on the current point.
    virtual void drawCube    (const float& radius, const RGBAColor& colour, const int& subd=16) = 0 ;

    virtual void drawCylinder(const Vector3& p1, const Vector3 &p2, float radius, const RGBAColor& colour,  int subd=16) = 0 ;

    virtual void drawCapsule(const Vector3& p1, const Vector3 &p2, float radius, const RGBAColor& colour,  int subd=16) = 0 ;

    virtual void drawArrow   (const Vector3& p1, const Vector3 &p2, float radius, const RGBAColor& colour,  int subd=16) = 0 ;
    virtual void drawArrow   (const Vector3& p1, const Vector3 &p2, float radius, float coneLength, const RGBAColor& colour,  int subd=16) = 0 ;
    virtual void drawArrow   (const Vector3& p1, const Vector3 &p2, float radius, float coneLength, float coneRadius, const RGBAColor& color,  int subd=16) = 0;

    /// Draw a cross (3 lines) centered on p
    virtual void drawCross(const Vector3&p, float length, const RGBAColor& colour) = 0;

    /// Draw a plus sign of size one centered on the current point.
    virtual void drawPlus    (const float& radius, const RGBAColor& colour, const int& subd=16) = 0 ;

    virtual void drawPoint(const Vector3 &p, const RGBAColor &c) = 0 ;
    virtual void drawPoint(const Vector3 &p, const Vector3 &n, const RGBAColor &c) = 0 ;

    virtual void drawTriangle(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,
            const Vector3 &normal) = 0 ;
    virtual void drawTriangle(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,
            const Vector3 &normal, const RGBAColor &c) = 0 ;
    virtual void drawTriangle(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,
            const Vector3 &normal,
            const RGBAColor &c1, const RGBAColor &c2, const RGBAColor &c3) = 0 ;
    virtual void drawTriangle(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,
            const Vector3 &normal1, const Vector3 &normal2, const Vector3 &normal3,
            const RGBAColor &c1, const RGBAColor &c2, const RGBAColor &c3) = 0 ;

    virtual void drawQuad(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,const Vector3 &p4,
            const Vector3 &normal) = 0 ;
    virtual void drawQuad(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,const Vector3 &p4,
            const Vector3 &normal, const RGBAColor &c) = 0 ;
    virtual void drawQuad(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,const Vector3 &p4,
            const Vector3 &normal,
            const RGBAColor &c1, const RGBAColor &c2, const RGBAColor &c3, const RGBAColor &c4) = 0 ;
    virtual void drawQuad(const Vector3 &p1,const Vector3 &p2,const Vector3 &p3,const Vector3 &p4,
            const Vector3 &normal1, const Vector3 &normal2, const Vector3 &normal3, const Vector3 &normal4,
            const RGBAColor &c1, const RGBAColor &c2, const RGBAColor &c3, const RGBAColor &c4) = 0 ;
    virtual void drawQuads(const std::vector<Vector3> &points, const RGBAColor& colour) = 0 ;
    virtual void drawQuads(const std::vector<Vector3> &points, const std::vector<RGBAColor>& colours) = 0 ;

    virtual void drawTetrahedron(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const RGBAColor &colour) = 0 ;
    virtual void drawTetrahedra(const std::vector<Vector3> &points, const RGBAColor& colour) = 0;
    //Scale each tetrahedron
    virtual void drawScaledTetrahedra(const std::vector<Vector3> &points, const RGBAColor& colour, const float scale) = 0;

    virtual void drawHexahedron(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3,
        const Vector3 &p4, const Vector3 &p5, const Vector3 &p6, const Vector3 &p7, const RGBAColor &colour) = 0;
    virtual void drawHexahedra(const std::vector<Vector3> &points, const RGBAColor& colour) = 0;
    //Scale each hexahedron
    virtual void drawScaledHexahedra(const std::vector<Vector3> &points, const RGBAColor& colour, const float scale) = 0;

    virtual void drawSphere( const Vector3 &p, float radius) = 0 ;
    virtual void drawSphere(const Vector3 &p, float radius, const RGBAColor& colour) = 0;
    virtual void drawEllipsoid(const Vector3 &p, const Vector3 &radii) = 0;

    virtual void drawBoundingBox( const Vector3 &min, const Vector3 &max, float size = 1.0 ) = 0;

    virtual void draw3DText(const Vector3 &p, float scale, const RGBAColor &color, const char* text) = 0;
    virtual void draw3DText_Indices(const std::vector<Vector3> &positions, float scale, const RGBAColor &color) = 0;
    /// @}

    /// @name Transformation methods.
    /// @{
    virtual void pushMatrix() = 0;

    virtual void popMatrix() =  0;

    virtual void multMatrix(float*  ) = 0;

    virtual void scale(float ) = 0;
    virtual void translate(float x, float y, float z) = 0;
    /// @}

    /// @name Drawing style methods.
    virtual void setMaterial(const RGBAColor &colour) = 0 ;

    virtual void resetMaterial(const RGBAColor &colour) = 0 ;
    virtual void resetMaterial() = 0 ;

    virtual void setPolygonMode(int _mode, bool _wireframe) = 0 ;

    virtual void setLightingEnabled(bool _isAnabled) = 0 ;
    /// @}

    virtual void enableBlending() = 0;
    virtual void disableBlending() = 0;

    virtual void enableLighting() = 0;
    virtual void disableLighting() = 0;

    virtual void enableDepthTest() = 0;
    virtual void disableDepthTest() = 0;

    /// @name States (save/restore)
    virtual void saveLastState() = 0;
    virtual void restoreLastState() = 0;

    /// @name Overlay methods

    /// draw 2D text at position (x,y) from top-left corner
    virtual void writeOverlayText( int x, int y, unsigned fontSize, const RGBAColor &color, const char* text ) = 0;

    /// Allow a variable depth offset for polygon drawing
    virtual void enablePolygonOffset(float factor, float units) = 0;
    /// Remove variable depth offset for polygon drawing
    virtual void disablePolygonOffset() = 0;

    // @name Color Buffer method
    virtual void readPixels(int x, int y, int w, int h, float* rgb, float* z = nullptr) = 0;
    /// @}

    virtual void clear() {}

    /// Compatibility wrapper functions 
    using Vec4f = sofa::defaulttype::Vec4f;
#define DISABLE_VEC4F \
    [[deprecated("This function has been deprecated in #PR 1626 and then disabled starting from the v21.06 release." \
    "Vec4f defining a color is obsolete, use RGBAColor instead.")]]


    // Necessary to not break existing code
    // as std::vector<RGBAColor> is not a std::vector<Vec4f>
    DISABLE_VEC4F
    void drawPoints(const std::vector<Vector3>& points, float size, const std::vector<Vec4f>& colour) = delete;
    
    DISABLE_VEC4F
    void drawLines(const std::vector<Vector3>& points, float size, const std::vector<Vec4f>& colours) = delete;

    DISABLE_VEC4F
    void drawTriangles(const std::vector<Vector3>& points, const std::vector< Vec3i >& index, const std::vector<Vector3>& normal, const std::vector<Vec4f>& colour) = delete;


    DISABLE_VEC4F
    void drawTriangles(const std::vector<Vector3>& points, const std::vector< Vec4f >& colour) = delete;

    DISABLE_VEC4F
    void drawTriangles(const std::vector<Vector3>& points,
            const std::vector<Vector3>& normal,
            const std::vector< Vec4f >& colour) = delete;

    DISABLE_VEC4F
    void drawQuads(const std::vector<Vector3>& points, const std::vector<Vec4f>& colours) = delete;

    DISABLE_VEC4F
    void drawPoints(const std::vector<Vector3>& points, float size, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawLine(const Vector3& p1, const Vector3& p2, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawInfiniteLine(const Vector3& point, const Vector3& direction, const Vec4f& color) = delete;

    DISABLE_VEC4F
    void drawLines(const std::vector<Vector3>& points, float size, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawLines(const std::vector<Vector3>& points, const std::vector< Vec2i >& index, float size, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawLineStrip(const std::vector<Vector3>& points, float size, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawLineLoop(const std::vector<Vector3>& points, float size, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawDisk(float radius, double from, double to, int resolution, const Vec4f& color) = delete;

    DISABLE_VEC4F
    void drawCircle(float radius, float lineThickness, int resolution, const Vec4f& color) = delete;

    DISABLE_VEC4F
    void drawTriangles(const std::vector<Vector3>& points, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawTriangles(const std::vector<Vector3>& points, const Vector3& normal, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawTriangles(const std::vector<Vector3>& points,
        const std::vector< Vec3i >& index,
        const std::vector<Vector3>& normal,
        const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawTriangleStrip(const std::vector<Vector3>& points,
        const std::vector<Vector3>& normal,
        const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawTriangleFan(const std::vector<Vector3>& points,
        const std::vector<Vector3>& normal,
        const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawFrame(const Vector3& position, const Quaternion& orientation, const Vec3f& size, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawSpheres(const std::vector<Vector3>& points, const std::vector<float>& radius, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawSpheres(const std::vector<Vector3>& points, float radius, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawFakeSpheres(const std::vector<Vector3>& points, const std::vector<float>& radius, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawFakeSpheres(const std::vector<Vector3>& points, float radius, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawCone(const Vector3& p1, const Vector3& p2, float radius1, float radius2, const Vec4f& colour, int subd = 16) = delete;

    DISABLE_VEC4F
    void drawCube(const float& radius, const Vec4f& colour, const int& subd = 16) = delete;

    DISABLE_VEC4F
    void drawCylinder(const Vector3& p1, const Vector3& p2, float radius, const Vec4f& colour, int subd = 16) = delete;

    DISABLE_VEC4F
    void drawCapsule(const Vector3& p1, const Vector3& p2, float radius, const Vec4f& colour, int subd = 16) = delete;

    DISABLE_VEC4F
    void drawArrow(const Vector3& p1, const Vector3& p2, float radius, const Vec4f& colour, int subd = 16) = delete;

    DISABLE_VEC4F
    void drawArrow(const Vector3& p1, const Vector3& p2, float radius, float coneLength, const Vec4f& colour, int subd = 16) = delete;

    DISABLE_VEC4F
    void drawArrow(const Vector3& p1, const Vector3& p2, float radius, float coneLength, float coneRadius, const Vec4f& color, int subd = 16) = delete;

    DISABLE_VEC4F
    void drawCross(const Vector3& p, float length, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawPlus(const float& radius, const Vec4f& colour, const int& subd = 16) = delete;

    DISABLE_VEC4F
    void drawPoint(const Vector3& p, const Vec4f& c) = delete;

    DISABLE_VEC4F
    void drawPoint(const Vector3& p, const Vector3& n, const Vec4f& c) = delete;

    DISABLE_VEC4F
    void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3,
        const Vector3& normal,
        const Vec4f& c1, const Vec4f& c2, const Vec4f& c3) = delete;
    DISABLE_VEC4F
    void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3,
        const Vector3& normal1, const Vector3& normal2, const Vector3& normal3,
        const Vec4f& c1, const Vec4f& c2, const Vec4f& c3) = delete;

    DISABLE_VEC4F
    void drawQuad(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4,
        const Vector3& normal, const Vec4f& c) = delete;

    DISABLE_VEC4F
    void drawQuad(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4,
        const Vector3& normal1, const Vector3& normal2, const Vector3& normal3, const Vector3& normal4,
        const Vec4f& c1, const Vec4f& c2, const Vec4f& c3, const Vec4f& c4) = delete;

    DISABLE_VEC4F
    void drawQuads(const std::vector<Vector3>& points, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawTetrahedron(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawTetrahedra(const std::vector<Vector3>& points, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawScaledTetrahedra(const std::vector<Vector3>& points, const Vec4f& colour, const float scale) = delete;

    DISABLE_VEC4F
    void drawHexahedron(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3,
        const Vector3& p4, const Vector3& p5, const Vector3& p6, const Vector3& p7, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawHexahedra(const std::vector<Vector3>& points, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void drawScaledHexahedra(const std::vector<Vector3>& points, const Vec4f& colour, const float scale) = delete;

    DISABLE_VEC4F
    void drawSphere(const Vector3& p, float radius, const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void draw3DText(const Vector3& p, float scale, const Vec4f& color, const char* text) = delete;

    DISABLE_VEC4F
    void draw3DText_Indices(const std::vector<Vector3>& positions, float scale, const Vec4f& color) = delete;

    DISABLE_VEC4F
    void setMaterial(const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void resetMaterial(const Vec4f& colour) = delete;

    DISABLE_VEC4F
    void writeOverlayText(int x, int y, unsigned fontSize, const Vec4f& color, const char* text) = delete;

#undef DISABLE_VEC4F

};

} // namespace sofa::helper::visual
