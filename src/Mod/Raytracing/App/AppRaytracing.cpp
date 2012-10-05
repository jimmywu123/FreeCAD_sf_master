/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <App/Application.h>

#include "RenderCameraPy.h"


#include "RayFeature.h"
#include "RayProject.h"
#include "RaySegment.h"

#include "RenderFeatureGroup.h"
#include "renderer/lux/LuxRender.h"
#include "RenderFeature.h"
#include "Appearances.h"

extern struct PyMethodDef Raytracing_methods[];

PyDoc_STRVAR(module_Raytracing_doc,
"This module is the Raytracing module.");

extern "C" {
void RaytracingExport initRaytracing()
{
    // load dependent module
    try {
        Base::Interpreter().loadModule("Part");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }
     PyObject* raytracingModule = Py_InitModule3("Raytracing", Raytracing_methods, module_Raytracing_doc);   /* mod name, table ptr */

      // Add Types to module
      Base::Interpreter().addType(&Raytracing::RenderCameraPy::Type,raytracingModule,"RenderCamera");

      Raytracing::RenderCamera        ::init();
      Raytracing::RenderFeature       ::init();
      Raytracing::RenderFeaturePython ::init();
      Raytracing::LuxRender           ::init();

      Raytracing::RenderFeatureGroup  ::init();
      Raytracing::PropertyRenderMaterialList::init();
      Raytracing::RenderMaterial::init();

      Raytracing::RaySegment          ::init();
      Raytracing::RayFeature          ::init();
      Raytracing::RayProject          ::init();


      // Load all the library materials
      // TODO we need enable Appearances to scan recursivly - temporarily use Lux as default directory
      std::string matPath = App::Application::getResourceDir() + "Mod/Raytracing/Materials/Lux";
      Raytracing::Appearances().setUserMaterialsPath(matPath.c_str());
      Raytracing::Appearances().scanMaterials();

      Base::Console().Log("Loading Raytracing module... done\n");
}

} // extern "C" {
