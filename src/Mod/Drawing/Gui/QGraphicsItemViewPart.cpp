/***************************************************************************
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
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

#include "PreCompiled.h"
#ifndef _PreComp_
#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QPainterPathStroker>
#include <QPainter>
#include <QTextOption>
#include <strstream>
#include <cmath>
#endif

#include <Base/Console.h>
#include "../App/FeatureViewPart.h"
#include "QGraphicsItemViewPart.h"

using namespace DrawingGui;

QGraphicsItemEdge::QGraphicsItemEdge(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
    if(ref > 0) {
        this->setAcceptHoverEvents(true);
    }
    
    strokeWidth = 1.;
    showHidden = false;
    highlighted = false;
    hPen.setStyle(Qt::DashLine);
    vPen.setStyle(Qt::SolidLine);
    vPen.setCosmetic(false);
    hPen.setCosmetic(false);
    hPen.setColor(Qt::gray);
}

void QGraphicsItemEdge::setVisiblePath(const QPainterPath &path) {  
    prepareGeometryChange();
    this->vPath = path;   
    bb = shape().controlPointRect();
    update(); 
}

void QGraphicsItemEdge::setHiddenPath(const QPainterPath &path) {  
    prepareGeometryChange();
    this->hPath = path;   
    bb = shape().controlPointRect();
    update(); 
}
    
QRectF QGraphicsItemEdge::boundingRect() const {
        return shape().controlPointRect();

}

bool QGraphicsItemEdge::contains(const QPointF &point)
{
    return shape().contains(point);
}

QPainterPath QGraphicsItemEdge::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(strokeWidth);
    
    // Combine paths
    QPainterPath p;
    p.addPath(vPath);
    
    if(showHidden)
        p.addPath(hPath);

    return stroker.createStroke(p);
}

void QGraphicsItemEdge::setHighlighted(bool state) 
{
    highlighted = state;
    if(highlighted) {
        vPen.setColor(Qt::blue); 
        hPen.setColor(Qt::blue);
    } else {
        vPen.setColor(Qt::black);
        hPen.setColor(Qt::gray);
    }
    update(); 
}

QVariant QGraphicsItemEdge::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        if(isSelected()) {
            vPen.setColor(Qt::blue);
            hPen.setColor(Qt::blue);
        } else {
            vPen.setColor(Qt::black);
            hPen.setColor(Qt::gray);

        }
        update();
    }
    return QGraphicsItem::itemChange(change, value);
}
 
void QGraphicsItemEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  Base::Console().Log("hover %f \n",strokeWidth);
  
    vPen.setColor(Qt::blue);
    hPen.setColor(Qt::blue);
    update();
}

void QGraphicsItemEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{    
    QGraphicsItemView *view = dynamic_cast<QGraphicsItemView *> (this->parentItem());
    assert(view != 0);
    
    if(!isSelected() && !highlighted) {
        vPen.setColor(Qt::black);
        hPen.setColor(Qt::gray);
        update();
    }
}

void QGraphicsItemEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    painter->setPen(vPen);
    painter->setBrush(vBrush);
    painter->drawPath(vPath);
    
    if(showHidden) {
        painter->setPen(hPen);
        painter->setBrush(hBrush);
        painter->drawPath(hPath);
    }
//     QGraphicsPathItem::paint(painter, &myOption, widget);
}

// QGraphicsView - Face Features

QGraphicsItemFace::QGraphicsItemFace(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
    this->setAcceptHoverEvents(true);    
}

QVariant QGraphicsItemFace::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        if(isSelected()) {
            QPen pen = this->pen();
            pen.setColor(Qt::blue);
            this->setPen(pen);
        } else {
            QPen pen = this->pen();
            pen.setColor(Qt::gray);
            this->setPen(pen);
        }
        update();
    }
    return QGraphicsItem::itemChange(change, value);
}
 
void QGraphicsItemFace::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPen pen = this->pen();
    pen.setColor(Qt::blue);
    this->setPen(pen);
    update();
}

void QGraphicsItemFace::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{    
    QGraphicsItemView *view = dynamic_cast<QGraphicsItemView *> (this->parentItem());
    assert(view != 0);
    
    if(!isSelected() && !view->isSelected()) {
        QPen pen = this->pen();
        pen.setColor(Qt::black);
        this->setPen(pen);
        update();
    }
}

// Vertex Features
QGraphicsItemVertex::QGraphicsItemVertex(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
    this->setAcceptHoverEvents(true);    
}

QPainterPath QGraphicsItemVertex::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(this->pen().widthF());
    return stroker.createStroke(this->path());
}

QVariant QGraphicsItemVertex::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        if(isSelected()) {
            QBrush brush = this->brush();
            brush.setColor(Qt::blue);
            this->setBrush(brush);
        } else {
            QBrush brush = this->brush();
            brush.setColor(Qt::black);
            this->setBrush(brush);
        }
        update();
    }
    return QGraphicsItem::itemChange(change, value);
}
 
void QGraphicsItemVertex::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QBrush brush = this->brush();
    brush.setColor(Qt::blue);
    this->setBrush(brush);
    update();
}

void QGraphicsItemVertex::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{    
    QGraphicsItemView *view = dynamic_cast<QGraphicsItemView *> (this->parentItem());
    assert(view != 0);
    
    if(!isSelected() && !view->isSelected()) {
          QBrush brush = this->brush();
          brush.setColor(Qt::black);
          this->setBrush(brush);
        update();
    }
}

void QGraphicsItemVertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    QGraphicsPathItem::paint(painter, &myOption, widget);
}

// ########## QGraphic Item Part #############

QGraphicsItemViewPart::QGraphicsItemViewPart(const QPoint &pos, QGraphicsScene *scene) :QGraphicsItemView(pos, scene)                 
{
    setHandlesChildEvents(false);
    
    pen.setColor(QColor(150,150,150));
    
    this->setAcceptHoverEvents(true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
}

QGraphicsItemViewPart::~QGraphicsItemViewPart()
{
  
}

QPainterPath QGraphicsItemViewPart::drawPainterPath(DrawingGeometry::BaseGeom *baseGeom) const
{
    QPainterPath path;
    switch(baseGeom->geomType) {
        case DrawingGeometry::CIRCLE: {
          DrawingGeometry::Circle *geom = static_cast<DrawingGeometry::Circle *>(baseGeom);
          
          double x = geom->center.fX - geom->radius;
          double y = geom->center.fY - geom->radius;
          
          path.addEllipse(x, y, geom->radius * 2, geom->radius * 2);

        } break;
        case DrawingGeometry::ARCOFCIRCLE: {
          DrawingGeometry::AOC  *geom = static_cast<DrawingGeometry::AOC *>(baseGeom);

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - startAngle);

          double x = geom->center.fX - geom->radius;
          double y = geom->center.fY - geom->radius;
          
          path.arcMoveTo(x, y, geom->radius * 2, geom->radius * 2, -startAngle);
          path.arcTo(x, y, geom->radius * 2, geom->radius * 2, -startAngle, -spanAngle);
        } break;
        case DrawingGeometry::ELLIPSE: {
          DrawingGeometry::Ellipse *geom = static_cast<DrawingGeometry::Ellipse *>(baseGeom);

          double x = geom->center.fX - geom->radius;
          double y = geom->center.fY - geom->radius;
          
          path.addEllipse(x,y, geom->major * 2, geom->minor * 2);
        } break;
        case DrawingGeometry::ARCOFELLIPSE: {
          DrawingGeometry::AOE *geom = static_cast<DrawingGeometry::AOE *>(baseGeom);
          
          double startAngle = (geom->startAngle);
          double spanAngle =  (startAngle - geom->endAngle);
          double endAngle = geom->endAngle;
          
          double x = geom->center.fX - geom->major;
          double y = geom->center.fY - geom->minor;
          
          double maj = geom->major * 2 * cos(geom->angle * M_PI / 180);
          double min = geom->minor * 2 * sin(geom->angle * M_PI / 180);

          path.arcMoveTo(x, y, maj, min, startAngle);
          path.arcTo(x, y, maj, min, startAngle, spanAngle);

        } break;
        case DrawingGeometry::BSPLINE: {
          DrawingGeometry::BSpline *geom = static_cast<DrawingGeometry::BSpline *>(baseGeom);
         
          std::vector<DrawingGeometry::BezierSegment>::const_iterator it = geom->segments.begin();
          
          DrawingGeometry::BezierSegment startSeg = geom->segments.at(0);
          path.moveTo(startSeg.pnts[0].fX, startSeg.pnts[0].fY);
          Base::Vector2D prevContPnt = startSeg.pnts[1];
          
          for(int i = 0; it != geom->segments.end(); ++it, ++i) {
              DrawingGeometry::BezierSegment seg = *it;
              if(seg.poles == 4) {
//                   path.cubicTo(seg.pnts[1].fX,seg.pnts[1].fY, seg.pnts[2].fX, seg.pnts[2].fY, seg.pnts[3].fX, seg.pnts[3].fY);
              } else {
                  Base::Vector2D cPnt;
                  if(i == 0) {
                    prevContPnt.Set(startSeg.pnts[1].fX, startSeg.pnts[1].fX);
                  } else {
                    prevContPnt.Set(2 * startSeg.pnts[1].fX - prevContPnt.fX, 2 * startSeg.pnts[1].fY - prevContPnt.fY);
                  }

                  path.quadTo(prevContPnt.fX, prevContPnt.fY, seg.pnts[2].fX, seg.pnts[2].fY);

              }
            }          
        } break;
        case DrawingGeometry::GENERIC: {
          DrawingGeometry::Generic *geom = static_cast<DrawingGeometry::Generic *>(baseGeom);          

          path.moveTo(geom->points[0].fX, geom->points[0].fY);
          std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

          for(++it; it != geom->points.end(); ++it) {
              path.lineTo((*it).fX, (*it).fY);
          }
        } break;
        default:
          break;
      }
      return path;
}

void QGraphicsItemViewPart::drawViewPart()
{    
    
    // Iterate
    if(this->viewObject == 0 || !this->viewObject->isDerivedFrom(Drawing::FeatureViewPart::getClassTypeId()))
        return;
        
    Drawing::FeatureViewPart *part = dynamic_cast<Drawing::FeatureViewPart *>(this->viewObject);
   
    float lineWidth = part->LineWidth.getValue();
          
    QGraphicsItem *graphicsItem = 0;
    
#if 0
    // Draw Faces     
    const std::vector<DrawingGeometry::Face *> &faceGeoms = part->getFaceGeometry();
    const std::vector<int> &faceRefs = part->getFaceReferences();
    
    std::vector<DrawingGeometry::Face *>::const_iterator fit = faceGeoms.begin();
    
    QPen facePen;
    for(int i = 0 ; fit != faceGeoms.end(); ++fit, i++) {
        std::vector<DrawingGeometry::Wire *> faceWires = (*fit)->wires;
        QPainterPath facePath;
        for(std::vector<DrawingGeometry::Wire *>::iterator wire = faceWires.begin(); wire != faceWires.end(); ++wire) {
            QPainterPath wirePath;            
            for(std::vector<DrawingGeometry::BaseGeom *>::iterator baseGeom = (*wire)->geoms.begin(); baseGeom != (*wire)->geoms.end(); ++baseGeom) {
                wirePath.connectPath(drawPainterPath(*baseGeom));
            }
            wirePath.closeSubpath();
            facePath.addPath(wirePath);
        }
        
        QGraphicsItemFace *item = new QGraphicsItemFace(-1);
 
        item->setPath(facePath);        
//         item->setStrokeWidth(lineWidth);
        
        QBrush faceBrush(QBrush(QColor(0,0,255,40)));
        
        item->setBrush(faceBrush);
        facePen.setColor(Qt::black);
        item->setPen(facePen);
        graphicsItem = dynamic_cast<QGraphicsItem *>(item);
               
        if(graphicsItem) {          
            // Hide any edges that are hidden if option is set.
//             if((*fit)->extractType == DrawingGeometry::WithHidden && !part->ShowHiddenLines.getValue())
//                 graphicsItem->hide();
              
            this->addToGroup(graphicsItem);
            graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }      
    }
#endif

    graphicsItem = 0;

    // Draw Edges
    const std::vector<DrawingGeometry::BaseGeom *> &geoms = part->getEdgeGeometry();
    const std::vector<int> &refs = part->getEdgeReferences();
    std::vector<DrawingGeometry::BaseGeom *>::const_iterator it = geoms.begin();
    
    // Draw Edges      
    // iterate through all the geometries
    for(int i = 0 ; it != geoms.end(); ++it, i++) {

     

      
       // Attempt to find if a previous edge exists
      QGraphicsItemEdge *item = this->findRefEdge(refs.at(i));
      Base::Console().Log("Line Width %f", lineWidth);  
      if(!item) {
          item = new QGraphicsItemEdge(refs.at(i));
          if(part->ShowHiddenLines.getValue())
              item->setShowHidden(true);
      }
      item->setStrokeWidth(lineWidth);
      
      QPainterPath path;
      
      graphicsItem = dynamic_cast<QGraphicsItem *>(item);
      
      switch((*it)->geomType) {
        case DrawingGeometry::CIRCLE: {
          DrawingGeometry::Circle *geom = static_cast<DrawingGeometry::Circle *>(*it);
          path.addEllipse(geom->center.fX - geom->radius ,geom->center.fY - geom->radius, geom->radius * 2, geom->radius * 2);

        } break;
        case DrawingGeometry::ARCOFCIRCLE: {
          DrawingGeometry::AOC  *geom = static_cast<DrawingGeometry::AOC *>(*it);
          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - startAngle);

          path.arcMoveTo(geom->center.fX - geom->radius, geom->center.fY - geom->radius, geom->radius * 2, geom->radius * 2, startAngle);
          path.arcTo(geom->center.fX - geom->radius, geom->center.fY - geom->radius, geom->radius * 2, geom->radius * 2, startAngle, abs(spanAngle));

        } break;
        case DrawingGeometry::ELLIPSE: {
          DrawingGeometry::Ellipse *geom = static_cast<DrawingGeometry::Ellipse *>(*it);
        
          path.addEllipse(geom->center.fX - geom->radius,geom->center.fY - geom->radius, geom->major * 2, geom->minor * 2);
          
        } break;
        case DrawingGeometry::ARCOFELLIPSE: {
          DrawingGeometry::AOE *geom = static_cast<DrawingGeometry::AOE *>(*it);

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - geom->startAngle);
          double endAngle = geom->endAngle;

          
          Base::Console().Log("(C <%f, %f> rot %f, SA %f, EA %f, SA %f Maj %f Min %f\n",
          geom->center.fX, 
          geom-> center.fY,
          geom->angle, startAngle, endAngle, spanAngle, geom->major, geom->minor);
          
          // Create a temporary painterpath since we are applying matrix transformation
          QPainterPath tmp;
          tmp.arcMoveTo(-geom->major, -geom->minor, geom->major * 2, geom->minor * 2, startAngle);
          tmp.arcTo(-geom->major,     -geom->minor, geom->major * 2, geom->minor * 2, startAngle, spanAngle);
          
          QMatrix mat;
          mat.translate(+geom->center.fX, +geom->center.fY).rotate(geom->angle);

          // Add path to existing
          path.addPath(mat.map(tmp));   

        } break;
        case DrawingGeometry::BSPLINE: {
          DrawingGeometry::BSpline *geom = static_cast<DrawingGeometry::BSpline *>(*it);
                   
          std::vector<DrawingGeometry::BezierSegment>::const_iterator it = geom->segments.begin();
          
          DrawingGeometry::BezierSegment startSeg = geom->segments.at(0);
          path.moveTo(startSeg.pnts[0].fX, startSeg.pnts[0].fY);
          Base::Vector2D prevContPnt = startSeg.pnts[1];
          
          for(int i = 0; it != geom->segments.end(); ++it, ++i) {
              DrawingGeometry::BezierSegment seg = *it;
              if(seg.poles == 4) {
                   path.cubicTo(seg.pnts[1].fX,seg.pnts[1].fY, seg.pnts[2].fX, seg.pnts[2].fY, seg.pnts[3].fX, seg.pnts[3].fY);
              } else {
                  Base::Vector2D cPnt;
                  if(i == 0) {
                    prevContPnt.Set(startSeg.pnts[1].fX, startSeg.pnts[1].fX);
                  } else {
                    prevContPnt.Set(2 * startSeg.pnts[1].fX - prevContPnt.fX, 2 * startSeg.pnts[1].fY - prevContPnt.fY);
                  }

                  path.quadTo(prevContPnt.fX, prevContPnt.fY, seg.pnts[2].fX, seg.pnts[2].fY);

              }
            }
          
        } break;
        case DrawingGeometry::GENERIC: {

          DrawingGeometry::Generic *geom = static_cast<DrawingGeometry::Generic *>(*it);

          path.moveTo(geom->points[0].fX, geom->points[0].fY);
          std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

          for(++it; it != geom->points.end(); ++it) {
              path.lineTo((*it).fX, (*it).fY);
          }

        } break;
        default:
          delete item;
          graphicsItem = 0;
          break;
      }
      
      if(graphicsItem) {
          if((*it)->extractType == DrawingGeometry::WithHidden) {
              QPainterPath hPath  = item->getHiddenPath();
              hPath.addPath(path);
              item->setHiddenPath(hPath);
          } else { 
              QPainterPath vPath  = item->getVisiblePath();
              vPath.addPath(path);
              item->setVisiblePath(vPath);
          }
            
          this->addToGroup(graphicsItem);
          
          // Don't allow selection for any edges with no references
          if(refs.at(i) > 0) {
              graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
          }
      }
    }
    
    graphicsItem = 0;
    // Draw Vertexs:
    const std::vector<DrawingGeometry::Vertex *> &verts = part->getVertexGeometry();
    const std::vector<int> &vertRefs = part->getVertexReferences();
    
    std::vector<DrawingGeometry::Vertex *>::const_iterator vert = verts.begin();
    
    QBrush vertBrush(QBrush(QColor(0,0,0,255)));        
          
     // iterate through all the geometries
    for(int i = 0 ; vert != verts.end(); ++vert, i++) {
          DrawingGeometry::Vertex *myVertex = *vert;
          QGraphicsItemVertex *item = new QGraphicsItemVertex(vertRefs.at(i));
          QPainterPath path;
          item->setBrush(vertBrush);
          path.addEllipse(0 ,0, 1, 1);
          item->setPath(path);
          item->setPos(myVertex->pnt.fX - 0.5, myVertex->pnt.fY - 0.5);
          item->setFlag(QGraphicsItem::ItemIsSelectable, true);
          this->addToGroup(item);
    }
}

QGraphicsItemEdge * QGraphicsItemViewPart::findRefEdge(int idx)
{
    QList<QGraphicsItem *> items = this->childItems();
    for(QList<QGraphicsItem *>::iterator it = items.begin(); it != items.end(); it++) {
        QGraphicsItemEdge *edge = dynamic_cast<QGraphicsItemEdge *>(*it);
        if(edge && edge->getReference() == idx) {
            return edge;
        }
    }
    return 0;
}
QVariant QGraphicsItemViewPart::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        QColor color;
        if(isSelected()) { 
          color.setRgb(0,0,255);
          pen.setColor(color);

        } else {
          color.setRgb(0,0,0);
          pen.setColor(QColor(150,150,150)); // Drawing Border
        }
        
        QList<QGraphicsItem *> items = this->childItems();
          for(QList<QGraphicsItem *>::iterator it = items.begin(); it != items.end(); ++it) {

              QGraphicsItemEdge *edge = dynamic_cast<QGraphicsItemEdge *>(*it);
              QGraphicsItemVertex *vert = dynamic_cast<QGraphicsItemVertex *>(*it);
              if(edge) {
                  edge->setHighlighted(isSelected());
              } else if(vert){
                  QBrush brush = vert->brush();
                  brush.setColor(color);
                  vert->setBrush(brush);
              }
          }
        update();
    }
    return QGraphicsItemView::itemChange(change, value);
}

void QGraphicsItemViewPart::setViewPartFeature(Drawing::FeatureViewPart *obj)
{
    if(obj == 0)
        return;
    
    this->setViewFeature(static_cast<Drawing::FeatureView *>(obj));
    this->drawViewPart();
      // Set the QGraphicsItemGroup Properties based on the FeatureView
    float x = obj->X.getValue();
    float y = obj->Y.getValue();
    this->setPos(x, y);
    Q_EMIT dirty();
}

void QGraphicsItemViewPart::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // TODO don't like this but only solution at the minute
    if(this->shape().contains(event->pos())) {
        pen.setColor(Qt::blue);        
    }
    update();
}

void QGraphicsItemViewPart::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isSelected()) {
        pen.setColor(QColor(150,150,150));
        update();
    }
}

QPainterPath QGraphicsItemViewPart::shape() const {
    QPainterPath path;
    QRectF box = this->boundingRect().adjusted(2,2,-2,-2);
    path.addRect(box);
    QPainterPathStroker stroker;
    stroker.setWidth(5.f);
    return stroker.createStroke(path);
}

QRectF QGraphicsItemViewPart::boundingRect() const
{
    return QGraphicsItemView::boundingRect().adjusted(-5,-5,5,5);
}
void QGraphicsItemViewPart::drawBorder(QPainter *painter){
  QRectF box = this->boundingRect().adjusted(2,2,-2,-2);
  // Save the current painter state and restore at end
  painter->save(); 
  QPen myPen = pen;
  myPen.setStyle(Qt::DashLine);
  myPen.setWidth(0.3);
  painter->setPen(myPen);
  QString name = QString::fromAscii(this->viewObject->Label.getValue());
  
  QFont font;
  font.setPointSize(3.f);
  painter->setFont(font);
  QFontMetrics fm(font);
  
  QPointF pos = box.center();
  pos.setY(pos.y() + box.height() / 2. - 3.);
  
  pos.setX(pos.x() - fm.width(name) / 2.);
  painter->drawText(pos, name);
  painter->drawRect(box);
  
  painter->restore();
}

void QGraphicsItemViewPart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    
    this->drawBorder(painter);
    QGraphicsItemView::paint(painter, &myOption, widget);
}

#include "moc_QGraphicsItemViewPart.cpp"