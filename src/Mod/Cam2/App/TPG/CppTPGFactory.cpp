/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#include "CppTPGFactory.h"

namespace Cam {

CppTPGFactory::CppTPGFactory() {
    // TODO Auto-generated constructor stub

}

CppTPGFactory::~CppTPGFactory() {
    // TODO Auto-generated destructor stub
}



/**
 * Get a vector of all C++ TPG's that are known about
 */
std::vector<TPGDescriptor*>* CppTPGFactory::getDescriptors()
{
    return NULL;
}

/**
 * Get an instance of the given TPG id.
 * Note: maybe we don't need this with the make method on the descriptor
 */
//TPG* CppTPGFactory::getTPG(QString id);

/**
 * Searches the C++ TPG Plugin directory for Shared objects that implement the required API.
 */
void CppTPGFactory::scanPlugins()
{

}

} /* namespace CamGui */
