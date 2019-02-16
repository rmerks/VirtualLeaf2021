/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#ifndef _MODELCATALOGUE_H_
#define _MODELCATALOGUE_H_

#include "warning.h"
#include "cell.h"
#include "mesh.h"
#include "simplugin.h"
#include "canvas.h"
#include <QPluginLoader>
#include <Q3PopupMenu>
#include <QAction>
#include <QDir>
#include <QApplication>
#include <QObject>
#include <QAction>
#include <QMenu>

class ModelCatalogue : public QObject {
  Q_OBJECT
    public:
  ModelCatalogue(Mesh *mesh, MainBase *mainwin, const char *model); 	
  void LoadPlugins(); 
  void LoadPlugin(const char *model);

  void InstallFirstModel();
  void PopulateModelMenu();	

  public slots:
  void InstallModel(SimPluginInterface *model);	
  void InstallModel(QAction *modelaction);
 private:
  QVector<SimPluginInterface *> models;
  Mesh *mesh;
  MainBase *mainwin;
};

#endif

/* finis */
