/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2005     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "../../PreCompiled.h"

#ifndef _PreComp_
# include <BRep_Tool.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <Poly_Triangulation.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS_Face.hxx>
# include <sstream>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>


#include "LuxRender.h"
#include "LuxRenderProcess.h"

using Base::Console;

using namespace Raytracing;
using namespace std;


// #SAMPLE FOR LUX RENDERER!
// #Global Information
// LookAt 0 10 100 0 -1 0 0 1 0
// Camera "perspective" "float fov" [30]
// 
// Film "fleximage"
// "integer xresolution" [200] "integer yresolution" [200]
// 
// PixelFilter "mitchell" "float xwidth" [2] "float ywidth" [2]
// 
// Sampler "lowdiscrepancy" "string pixelsampler" ["lowdiscrepancy"]
// 
// #Scene Specific Information
// WorldBegin
// 
// AttributeBegin
//         CoordSysTransform "camera"
//         LightSource "distant"
//                 "point from" [0 0 0] "point to" [0 0 1]
//                 "color L" [3 3 3]
// AttributeEnd
// 
// AttributeBegin
//         Rotate 135 1 0 0
// 
//         Texture "checks" "color" "checkerboard"
//                 "float uscale" [4] "float vscale" [4]
//                 "color tex1" [1 0 0] "color tex2" [0 0 1]
// 
//         Material "matte"
//                 "texture Kd" "checks"
//         Shape "disk" "float radius" [20] "float height" [-1]
// AttributeEnd
// 
// WorldEnd

LuxRender::LuxRender(void){}
LuxRender::~LuxRender(void){}

void LuxRender::generateScene()
{
    QTextStream out;
    if(!getOutputStream(out))
      return;

    out << "#Global Information:" << endl
        << genCamera(camera).c_str()
        << genRenderProps().c_str()
        << "#Scene Specific Information:" << endl
        << "WorldBegin" << endl;

    for (std::vector<RenderLight *>::iterator it = lights.begin(); it != lights.end(); ++it) {
        out << genLight(*it).c_str();
    }
    for (std::vector<RenderPart *>::iterator it = parts.begin(); it != parts.end(); ++it) {
        out << genObject(*it).c_str();
    }

    out << "\nWorldEnd" << endl;
}


// Renderer "sampler"
// 
// Sampler "metropolis"
//         "float largemutationprob" [0.400000005960464]
//         "bool usevariance" ["false"]
// 
// Accelerator "qbvh"
// 
// SurfaceIntegrator "bidirectional"
//         "integer eyedepth" [48]
//         "integer lightdepth" [48]
// 
// VolumeIntegrator "multi"
//         "float stepsize" [1.000000000000000]
// 
// PixelFilter "mitchell"
//         "bool supersample" ["true"]
        
std::string LuxRender::genRenderProps()
{
    std::stringstream out;
    out << "# Scene render Properties:" << endl
        << "Renderer \"sampler\"" << endl
        << "\nSampler \"metropolis\"" << endl
        << "\t \"float largemutationprob\" [0.400000005960464]" << endl
        << "\"bool usevariance\" [\"false\"]" << endl
        << "\nAccelerator \"qbvh\"" << endl
        << "SurfaceIntegrator \"bidirectional\""
        << "\t\"integer eyedepth\" [48]" << endl
        << "\t\"integer lightdepth\" [48]" << endl
        << "VolumeIntegrator \"multi\"" << endl
        << "\t\"float stepsize\" [1.000000000000000]"
        << "\nPixelFilter \"mitchell\"" << endl
        << "\t\"bool supersample\" [\"true\"]" << endl
        << "\nFilm \"fleximage\" \"integer xresolution\" [" << this->xRes << "] \"integer yresolution\" [" << yRes << "]" << endl
        << "PixelFilter \"mitchell\" \"float xwidth\" [2] \"float ywidth\" [2]" << endl;

    return out.str();
}

std::string LuxRender::genLight(RenderLight *light) const
{
    std::stringstream out;
    out <<  "\nAttributeBegin " << endl
        <<  "\tLightGroup \"default\"" << endl;

    // Switch the camera type
    switch(light->getType()) {
      case RenderLight::AREA: {

        const float * color = light->getColor();
        out << "\n\tMaterial \"matte\" \"color Kd\" [" << color[0] << " " << color[1] << " " << color[2] << "]" << endl;

        // Actual area light
        out << "\n\tAreaLightSource \"area\"" << endl
            << "\t\t\"float power\" [" << light->getPower() << "]" << endl
            << "\t\t\"color L\" [" << color[0] << " " << color[1] << " " << color[2] << "]" << endl;

        // Create the area light
        // Generate Points for plane centered around origin
        RenderAreaLight *areaLight = static_cast<RenderAreaLight *>(light);
        Base::Vector3d pnts[4];
        areaLight->generateGeometry(pnts);

        // More Efficient to create geometry straight away
        out << "Shape \"trianglemesh\"" << endl
            << "\t\"integer indices\" [0 2 3 0 3 1]" << endl
            << "\t\"point P\" [" << pnts[0][0] << " " << pnts[0][1] << " " << pnts[0][2] << " "
                                 << pnts[1][0] << " " << pnts[1][1] << " " << pnts[1][2] << " "
                                 << pnts[2][0] << " " << pnts[2][1] << " " << pnts[2][2] << " "
                                 << pnts[3][0] << " " << pnts[3][1] << " " << pnts[3][2] << "]" << endl
            << "\t\"string name\" [\"Lamp\"]" << endl;
      } break;
      case RenderLight::DISTANT: {
        out <<  "\tCoordSysTransform \"camera\"" << endl
            <<  "\n\tLightSource \"distant\"" << endl
            <<  "\t\t\"point from\" [0 0 0] \"point to\" [0 0 1]" << endl
            <<  "\t\t\"color L\" [3 3 3]" << endl;
      }
    }
    out <<  "AttributeEnd" << endl;

    return out.str();
}
std::string LuxRender::genCamera(RenderCamera *camera) const
{
    if(!camera)
        return std::string();

    std::string camType;
    // Switch the camera type
    switch(camera->Type) {
      case RenderCamera::ORTHOGRAPHIC:
        camType = "orthographic"; break;
      default:
      case RenderCamera::PERSPECTIVE:
        camType = "perspective"; break;
    }

    std::stringstream out;
    out << "\nLookAt " << camera->CamPos.x << " " << camera->CamPos.y << " " << camera->CamPos.z << " "
                     << camera->LookAt.x << " " << camera->LookAt.y << " " << camera->LookAt.z << " "
                     << camera->Up.x     << " " << camera->Up.y     << " " << camera->Up.z     << "\n " << endl;

    out << "# Camera Declaration and View Direction" << endl
        << "Camera \"" << camType << "\" \"float fov\" [50]" << endl;
    if(camera->Type == RenderCamera::PERSPECTIVE && camera->autofocus)
        out << "\t\"bool autofocus\" [\"true\"]" << endl;
    out << "\t\"float focaldistance\" [" << camera->focaldistance << "]" << endl;
    return out.str();
}

std::string LuxRender::genMaterial(RenderMaterial *mat)
{
    // Texture "checks" "color" "checkerboard"
    //         "float uscale" [4] "float vscale" [4]
    //         "color tex1" [1 0 0] "color tex2" [0 0 1]
    //
    // Material "matte"
    //         "texture Kd" "checks"
    // Texture "SolidColor" "color" "constant" "color value" [1.000 0.910 0.518]

    
    std::stringstream out;

    if(mat->getMaterial()->source == LibraryMaterial::BUILTIN) {
        out << "\nTexture \"SolidColor\" \"color\" \"constant\" \"color value\" [1.0 1.0 1.0]" << endl;
        out << "Material \"" << mat->getMaterial()->compat.toStdString() << "\"" << endl;
        QMap<QString, MaterialProperty *>::const_iterator it = mat->properties.constBegin();
        while (it != mat->properties.constEnd()) {
          switch(it.value()->getType()) {
            case MaterialParameter::BOOL: break;
            case MaterialParameter::FLOAT: {
              MaterialFloatProperty *prop = static_cast<MaterialFloatProperty *>(it.value());
              out << "\"float " << it.key().toStdString() << "\" [" << prop->getValue() <<  "]"; break;}
            default: break;
          }
          ++it;
        }
    
        // Add the parameters
        out << "\t\"texture Kd\" \"SolidColor\"" << endl;
    } else {
        //Open the filename and append
        QFile file(mat->getMaterial()->filename);
        if(!file.open(QFile::ReadOnly))
            return std::string("");

        QTextStream textStr(&file);
        while(!textStr.atEnd())
        {
          out << textStr.readLine().toStdString() << endl;
        }
    }

    return out.str();
}

std::string LuxRender::genObject(RenderPart *obj)
{
    //fMeshDeviation is a class variable
    Base::Console().Log("Meshing with Deviation: %f\n", obj->getMeshDeviation());

    const TopoDS_Shape &Shape = obj->getShape();
    TopExp_Explorer ex;
    BRepMesh_IncrementalMesh MESH(Shape,obj->getMeshDeviation());

    const char * name = obj->getName();
    // counting faces and start sequencer
    int l = 1;
    std::stringstream out;
    out << "\nObjectBegin \"" << name << "\"" << endl;

    // Generate the material
    out << genMaterial(obj->getMaterial());

    //Generate each face
    for (ex.Init(Shape, TopAbs_FACE); ex.More(); ex.Next(),l++) { 
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        out << genFace(aFace, l);
    }
    out << "ObjectEnd" << endl
        << "ObjectInstance \"" << name << "\"" << endl;
    return out.str();
}

std::string LuxRender::genFace(const TopoDS_Face& aFace, int index )
{
    // this block mesh the face and transfers it in a C array of vertices and face indexes
    Standard_Integer nbNodesInFace,nbTriInFace;
    gp_Vec* verts=0;
    gp_Vec* vertNorms=0;
    long* cons=0;

    transferToArray(aFace,&verts,&vertNorms,&cons,nbNodesInFace,nbTriInFace);
    if (!verts)
      return std::string();

    std::stringstream out;

    out << "#Face Number: " << index << endl
        << "AttributeBegin" << endl
        << "\tShape \"trianglemesh\" \"string name\" \"Face " << index << "\"" << endl;

    // Write the Vertex Points in order
    out << "\n\t\"point P\"" << endl
        << "\t[" << endl;
    for (int i=0; i < nbNodesInFace; i++) {
        out << "\t\t" <<  verts[i].X() << " " << verts[i].Y() << " " << verts[i].Z() << " " << endl;
    }
    out << "\t]" << endl; // End Property

    // Write the Normals in order
    out << "\n\t\"normal N\"" << endl
        << "\t[" << endl;
    for (int j=0; j < nbNodesInFace; j++) {
        out << "\t\t" <<  vertNorms[j].X() << " " << vertNorms[j].Y() << " " << vertNorms[j].Z() << " " << endl;
    }
    out << "\t]" << endl; // End Property

    // Write the Face Indices in order
    out << "\n\t\"integer indices\"" << endl
        << "\t[" << endl;
   for (int k=0; k < nbTriInFace; k++) {
        out << "\t\t" <<  cons[3*k] << " " << cons[3*k + 2] << " " << cons[3*k + 1] << endl;
    }
    out << "\t]" << endl; // End Property

    // End of Face
    out << "AttributeEnd\n" << endl;

    delete [] vertNorms;
    delete [] verts;
    delete [] cons;

    return out.str();
}

void LuxRender::render()
{
    // Check if there are any processes running;
    if(process && process->isActive())
        return;

    // Create a new Render Process
    LuxRenderProcess * process = new LuxRenderProcess();
    this->attachRenderProcess(process);
    Renderer::render();
}