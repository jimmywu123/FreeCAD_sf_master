/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <sstream>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>

#include "FeatureViewCollection.h"

using namespace Drawing;

//===========================================================================
// FeatureViewCollection
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureViewCollection, Drawing::FeatureView)

FeatureViewCollection::FeatureViewCollection()
{
    static const char *group = "Drawing view";
    ADD_PROPERTY_TYPE(Source    ,(0), group, App::Prop_None,"Shape to view");
    ADD_PROPERTY_TYPE(Views     ,(0), group, App::Prop_None,"Attached Views");

}

FeatureViewCollection::~FeatureViewCollection()
{
}

short FeatureViewCollection::mustExecute() const
{
    // If Tolerance Property is touched
    if(Views.isTouched() ||
       Source.isTouched()) {
        return 1;
   } else
        return Drawing::FeatureView::mustExecute();
}
int FeatureViewCollection::countChildren()
{
    //Count the children recursively if needed
    int numChildren = 0;

    const std::vector<App::DocumentObject *> &views = Views.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = views.begin(); it != views.end(); ++it) {

        App::DocumentObject *docObj = dynamic_cast<App::DocumentObject *>(*it);
        if(docObj->getTypeId().isDerivedFrom(Drawing::FeatureViewCollection::getClassTypeId())) {
            Drawing::FeatureViewCollection *viewCollection = dynamic_cast<Drawing::FeatureViewCollection *>(*it);
            numChildren += viewCollection->countChildren() + 1;
        } else {
            numChildren += 1;
        }
    }
    return numChildren;
}

void FeatureViewCollection::onDocumentRestored()
{
    // Rebuild the view
    this->execute();
}

/// get called by the container when a Property was changed
void FeatureViewCollection::onChanged(const App::Property* prop)
{
    if (prop == &Source ||
        prop == &Views){
        if (!this->isRestoring()) {
            Base::Console().Log("Views touched");
        }
    }
    Drawing::FeatureView::onChanged(prop);
}
App::DocumentObjectExecReturn *FeatureViewCollection::execute(void)
{
    // Rebuild the view
    const std::vector<App::DocumentObject *> &views = Views.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::DocumentObject *docObj = *it;
        if(docObj->getTypeId().isDerivedFrom(Drawing::FeatureView::getClassTypeId())) {
            Drawing::FeatureView *view = dynamic_cast<Drawing::FeatureView *>(*it);

            // Set scale factor of each view
            view->Scale.setValue(this->Scale.getValue());
        }
    }

    return Drawing::FeatureView::execute();
}