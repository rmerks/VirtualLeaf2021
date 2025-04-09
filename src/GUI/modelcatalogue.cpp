/*
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

#include <string>
#include "modelcatalogue.h"
#include <QVariant>
#include <QApplication>

static const std::string _module_id("$Id$");
extern bool useGUI;

ModelCatalogue::ModelCatalogue(Mesh *_mesh, MainBase *_mainwin, const char *model=0) {
  mesh = _mesh;
  mainwin = _mainwin;
  if (model) {
    cerr << "Loading model: " << model << endl;
    LoadPlugin(model);
  } else {
    cerr << "Loading all models." << endl;
    LoadPlugins();
  }
}

void ModelCatalogue::LoadPlugins() {

  QDir pluginDir(QApplication::applicationDirPath());
  QStringList plugin_filters; // filter for plugins, i.e "*.dll", "*.dylib"
#if defined(Q_OS_WIN) 
  if (pluginDir.dirName().toLower() =="debug" 
      ||pluginDir.dirName().toLower() =="release") 
    pluginDir.cdUp(); 
  plugin_filters << "*.dll";
#elif defined(Q_OS_MAC) 
  if (pluginDir.dirName() =="MacOS"){ 
    pluginDir.cdUp(); 
  } 
  plugin_filters << "*.dylib";
#endif 
  pluginDir.setNameFilters(plugin_filters);

  if (!pluginDir.cd("models")) {
    MyWarning::error("Directory 'models' not found! from: %S", pluginDir.absolutePath().toStdString().c_str());
  }


  //QVector<SimPluginInterface *> plugins;
  foreach (QString fileName, pluginDir.entryList(QDir::Files)){ 
    QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
    if (SimPluginInterface *plugin = 
	qobject_cast<SimPluginInterface *>(loader.instance())) {
      models.append(plugin); 
    } else {
      cerr << loader.errorString().toStdString().c_str() << endl;
      MyWarning::warning("Could not load model %s: %s",fileName.toStdString().c_str(), loader.errorString().toStdString().c_str());
    }
  }
  if (models.size()==0) {
    MyWarning::error("No models could be loaded.");
  }
}

void ModelCatalogue::LoadPlugin(const char *model) {


  QDir pluginDir(QApplication::applicationDirPath()); 
  QStringList plugin_filters; // filter for plugins, i.e "*.dll", "*.dylib"


#if defined(Q_OS_WIN) 
  if (pluginDir.dirName().toLower() =="debug" 
      ||pluginDir.dirName().toLower() =="release") 
    pluginDir.cdUp(); 
  //plugin_filters << "*.dll";
#elif defined(Q_OS_MAC) 
  if (pluginDir.dirName() =="MacOS"){ 
    pluginDir.cdUp(); 
  } 
  //plugin_filters << "*.dylib";
#endif
  plugin_filters << model;
  pluginDir.setNameFilters(plugin_filters);

  if (!pluginDir.cd("models")) {
	    MyWarning::error("Directory 'models' not found! from: %S", pluginDir.absolutePath().toStdString().c_str());
  }

  QStringList modelnames=pluginDir.entryList(QDir::Files);
  if (modelnames.empty()) {
    MyWarning::error("Model %s not found - hint: do not include path in filename.",model);
  }
  foreach (QString fileName, modelnames){ 
    QPluginLoader loader(pluginDir.absoluteFilePath(fileName)); 

    if (SimPluginInterface *plugin = 
	qobject_cast<SimPluginInterface *>(loader.instance())) {
      models.append(plugin); 
      //MyWarning::warning("Successfully loaded model %s",fileName.toStdString().c_str());
    } else {
      MyWarning::warning("Could not load plugin %s",fileName.toStdString().c_str());
    }
  }
}

void ModelCatalogue::InstallFirstModel() {
  InstallModel(models[0]);
}
void ModelCatalogue::PopulateModelMenu() {
  foreach (SimPluginInterface *model, models) {
    QAction *modelaction = new QAction(model->ModelID(), (Main *)mainwin); 
    QVariant data;
    data.setValue(model);
    modelaction->setData(data);
    ((Main *)mainwin)->modelmenu->addAction(modelaction);

  }
  connect(((Main *)mainwin)->modelmenu, SIGNAL(triggered(QAction *)), this, SLOT(InstallModel(QAction *)) );
}	

void ModelCatalogue::InstallModel(QAction *modelaction) {
  QVariant data = modelaction->data();
  SimPluginInterface *model = data.value<SimPluginInterface *>();
  cerr << "You chose model " << model->ModelID().toStdString().c_str() << "!\n";
  mesh->Clean();
  InstallModel(model);
}

void ModelCatalogue::InstallModel(SimPluginInterface *plugin) {

  // make sure both main and plugin use the same static datamembers (ncells, nchems...)
  mesh->Clean();
  plugin->SetCellsStaticDatamembers(CellBase::GetStaticDataMemberPointer());

  mesh->SetSimPlugin(plugin);
 

  Cell::SetNChem(plugin->NChem());
  plugin->SetParameters(&par);
  
  if (mainwin) {
    
    if (useGUI)  // only do this if we are running a GUI
      ((Main *)mainwin)->RefreshInfoBar();
    
    if (plugin->DefaultLeafML().isEmpty()) {
      mainwin->Init(0);
    } else {
      // locate LeafML file
      
      QDir pluginDir(QApplication::applicationDirPath()); 
      QStringList plugin_filters; // filter for plugins, i.e "*.dll", "*.dylib"
      
      
#if defined(Q_OS_WIN) 
      if (pluginDir.dirName().toLower() =="debug" 
	  ||pluginDir.dirName().toLower() =="release") 
	pluginDir.cdUp(); 
      //plugin_filters << "*.dll";
#elif defined(Q_OS_MAC) 
      if (pluginDir.dirName() =="MacOS"){ 
	pluginDir.cdUp(); 
      }
     
#endif
      bool leafDirFound = pluginDir.cd("data/leaves");
      // for all OS-es. Move from "bin" directory to root application folder.
      if (!leafDirFound && pluginDir.dirName() == "bin") {
	     pluginDir.cdUp();
	     leafDirFound = pluginDir.cd("data/leaves");
      }
      // last resort go one up and try there
      if (!leafDirFound) {
	     pluginDir.cdUp();
	     leafDirFound = pluginDir.cd("data/leaves");
      }
      if (!leafDirFound) {
	    MyWarning::warning("Directory 'data/leaves' not found! Cannot load LeafML file '%s'. Reverting to standard initial condition now...",plugin->DefaultLeafML().toStdString().c_str());
	    mainwin->Init(0);
      } else {
	    if (!pluginDir.exists(plugin->DefaultLeafML())) {
	      MyWarning::error("LeafML file '%s' not found - hint: is file in data/leaves folder? Reverting to standard initial condition now...",plugin->DefaultLeafML().toStdString().c_str());
	      mainwin->Init(0);
	    } else {
	     // Initialize simulation using default LeafML file referenced in plugin.
	     //mainwin->Init(0);
	     cerr << "Default LeafML: " << plugin->DefaultLeafML().toStdString().c_str() << endl;
	     mainwin->Init(pluginDir.absoluteFilePath(plugin->DefaultLeafML()).toStdString().c_str());
	   }
      }
    }
  }

}
