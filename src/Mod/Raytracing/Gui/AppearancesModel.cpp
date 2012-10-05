  /***************************************************************************
  *   Copyright (c) Luke Parry          (l.parry@warwick.ac.uk)    2012     *
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
#endif
 
#include "AppearancesModel.h"

using namespace RaytracingGui;

AppearancesModel::AppearancesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[MaterialIdRole] = "materialId";
    roles[LabelRole] = "label";
    roles[DescriptionRole] = "description";
    roles[PreviewFilenameRole] = "previewFilename";
    setRoleNames(roles);
}

void AppearancesModel::addLibraryMaterial(LibraryMaterial *mat)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_libMats << mat;
    endInsertRows();
}

int AppearancesModel::rowCount(const QModelIndex & parent) const {
    return m_libMats.count();
}

QVariant AppearancesModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_libMats.count())
        return QVariant();

    const LibraryMaterial *material = m_libMats[index.row()];
    
    if (role == MaterialIdRole)
        return material->id;
    else if (role == LabelRole)
        return material->label;
    else if (role == DescriptionRole)
        return material->description;
    else if (role == PreviewFilenameRole)
        return material->previewFilename;
    return QVariant();
}

#include "moc_AppearancesModel.cpp"