/***************************************************************************
 *   Copyright (c) 2004 J�rgen Riegel <juergen.riegel@web.de>              *
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

#ifndef DRAWINGGUI_VIEWPROVIDERPAGE_H
#define DRAWINGGUI_VIEWPROVIDERPAGE_H

#include <Gui/ViewProviderFeature.h>
#include <Gui/ViewProviderDocumentObjectGroup.h>

#include <QPointer>

namespace Drawing{
    class FeaturePage;
}

namespace DrawingGui {

class DrawingView;

class DrawingGuiExport ViewProviderDrawingPage : public Gui::ViewProviderDocumentObjectGroup
{
    PROPERTY_HEADER(DrawingGui::ViewProviderDrawingPage);

public:

    ViewProviderDrawingPage();  /// constructor
    ~ViewProviderDrawingPage(); /// destructor

    App::PropertyFloat         HintScale;
    App::PropertyFloat         HintOffsetX;
    App::PropertyFloat         HintOffsetY;



    virtual void attach(App::DocumentObject *);
    virtual void setDisplayMode(const char* ModeName);
    virtual bool useNewSelectionModel(void) const {return false;}
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;

    /// Claim all the views for the page
    std::vector<App::DocumentObject*> claimChildren(void) const;

    /// Is called by the tree if the user double click on the object
    virtual bool doubleClicked(void);
    void setupContextMenu(QMenu*, QObject*, const char*);
    virtual void updateData(const App::Property*);

    Drawing::FeaturePage* getPageObject() const;
    void unsetEdit(int ModNum);

protected:
    bool setEdit(int ModNum);
    bool onDelete(const std::vector<std::string> &subList);
    DrawingView* showDrawingView();

private:
    QPointer<DrawingView> view;
};

} // namespace DrawingGui

#endif // DRAWINGGUI_VIEWPROVIDERPAGE_H
