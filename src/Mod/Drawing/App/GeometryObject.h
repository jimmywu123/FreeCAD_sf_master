/***************************************************************************
 *   Copyright (c) 2012 Luke Parry <l.parry@warwick.ac.uk>                 *
 *                                                                         *
 *   This file is Drawing of the FreeCAD CAx development system.           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A DrawingICULAR PURPOSE.  See the      *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DRAWING_GEOMETRYOBJECT_H
#define _DRAWING_GEOMETRYOBJECT_H

#include <TopoDS_Shape.hxx>
#include <Base/Vector3D.h>
#include <string>
#include <vector>

#include "Geometry.h"

class HLRBRep_Algo;
class Handle_HLRBRep_Data;
class HLRBRep_EdgeData;
class TopoDS_Wire;

namespace DrawingGeometry
{

class BaseGeom;
/** Algo class for projecting shapes and creating SVG output of it
 */
class DrawingExport GeometryObject
{
public:
    /// Constructor
    GeometryObject();
    virtual ~GeometryObject();

    void clear();

    void setTolerance(double value);
    void setScale(double value);

    const std::vector<Vertex *>   & getVertexGeometry() const { return vertexGeom; };
    const std::vector<BaseGeom *> & getEdgeGeometry() const { return edgeGeom; };
    const std::vector<Face *>     & getFaceGeometry() const { return faceGeom; };
    
    const std::vector<int> & getVertexRefs() const { return vertexReferences; };
    const std::vector<int> & getEdgeRefs() const { return edgeReferences; };
    const std::vector<int> & getFaceRefs() const { return faceReferences; };
    
    DrawingGeometry::BaseGeom * projectEdge(const TopoDS_Shape &edge, const TopoDS_Shape &support, const Base::Vector3f &direction);
    DrawingGeometry::Vertex   * projectVertex(const TopoDS_Shape &vert, const TopoDS_Shape &support, const Base::Vector3f &direction);
    
    void extractGeometry(const TopoDS_Shape &input,const Base::Vector3f &direction, bool extractHidden = false);

protected:
    bool shouldDraw(const bool inFace, const int typ,HLRBRep_EdgeData& ed);
    bool isSameCurve(const BRepAdaptor_Curve &c1, const BRepAdaptor_Curve &c2) const;
  
    // Reimplements HLRBRep Drawing Algorithms to satisfy Drawing Workbench requirements
    void drawFace(const bool visible, const int typ, const int iface, Handle_HLRBRep_Data & DS, TopoDS_Shape& Result) const;
    void drawEdge(HLRBRep_EdgeData& ed, TopoDS_Shape& Result, const bool visible) const;
    
    void extractVerts(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, HLRBRep_EdgeData& ed, int ie, ExtractionType extractionType);
    void extractEdges(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, int type, bool visible, ExtractionType extractionType);
    void extractFaces(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, int type, bool visible, ExtractionType extractionType);

    int calculateGeometry(const TopoDS_Shape &input, ExtractionType extractionType, std::vector<BaseGeom *> &geoms);
    
    void createWire(const TopoDS_Shape &input, std::list<TopoDS_Wire> &wires) const;    
    TopoDS_Shape invertY(const TopoDS_Shape& shape);
    
    // Geometry
    std::vector<BaseGeom *> edgeGeom;
    std::vector<Vertex *> vertexGeom;
    std::vector<Face *> faceGeom;
    
    // Linked Edges and Faces to base object
    std::vector<int> vertexReferences;
    std::vector<int> edgeReferences;
    std::vector<int> faceReferences;

    double Tolerance;
    double Scale;
    HLRBRep_Algo *brep_hlr;
};

} //namespace DrawingGeometry

#endif