/***************************************************************************
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
 *   Copyright (c) 2012 Yorik van Havre <yorik@uncreated.net>              *
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

#ifndef _Drawing_FeatureViewAnnotation_h_
#define _Drawing_FeatureViewAnnotation_h_

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>

namespace Drawing
{

/** Base class of all View Features in the drawing module
 */
class DrawingExport FeatureViewAnnotation : public App::DocumentObject
{
    PROPERTY_HEADER(Drawing::FeatureViewAnnotation);

public:
    /// Constructor
    FeatureViewAnnotation();
    virtual ~FeatureViewAnnotation();

    App::PropertyString       Font;
    App::PropertyColor        TextColor;

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    virtual App::DocumentObjectExecReturn *execute(void);
    //@}

    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "DrawingGui::ViewProviderDrawingView";
    }
};

} //namespace Drawing
#endif