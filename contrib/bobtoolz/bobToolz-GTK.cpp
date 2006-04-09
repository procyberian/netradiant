/*
BobToolz plugin for GtkRadiant
Copyright (C) 2001 Gordon Biggans

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "libxml/parser.h"


#include "str.h"

#include "funchandlers.h"

#include "dialogs/dialogs-gtk.h"
#include "../../libs/cmdlib.h"

void BobToolz_construct()
{
}

void BobToolz_destroy()
{
}

// Radiant function table
_QERFuncTable_1			  __QERTABLENAME;
_QERShadersTable		  __SHADERSTABLENAME;			// vvvvvvvvvvvvvvvvvvvv
_QERQglTable				  __QGLTABLENAME;				// for path plotting (hooking to DBobView)
_QERUITable					  g_MessageTable;			// for path plotting (listening for update)
_QEREntityTable			  __ENTITYTABLENAME;
_QERBrushTable			  __BRUSHTABLENAME;
_QERPatchTable			  __PATCHTABLENAME;

// plugin name
char* PLUGIN_NAME = "bobToolz";

// commands in the menu
static char* PLUGIN_COMMANDS = "About...,-,Reset Textures...,PitOMatic,-,Vis Viewer,Brush Cleanup,Polygon Builder,Caulk Selection,-,Tree Planter,Drop Entity,Plot Splines,-,Merge Patches,Split patches,Turn edge";

// globals
GtkWidget *g_pRadiantWnd = NULL;

static const char *PLUGIN_ABOUT =	"bobToolz for SDRadiant\n"
									"by digibob (digibob@splashdamage.com)\n"
									"http://www.splashdamage.com\n\n"
									"Additional Contributors:\n"
									"MarsMattel, RR2DO2\n";

extern "C" const char* QERPlug_Init( void* hApp, void* pMainWidget ) {
	g_pRadiantWnd = (GtkWidget*)pMainWidget;

	return "bobToolz for GTKradiant";
}

extern "C" const char* QERPlug_GetName() {
	return PLUGIN_NAME;
}

extern "C" const char* QERPlug_GetCommandList() {
	return PLUGIN_COMMANDS;
}

extern "C" void QERPlug_Dispatch (const char *p, vec3_t vMin, vec3_t vMax, bool bSingleBrush) {
	LoadLists();

	if( string_equal_nocase( p, "brush cleanup" ) ) {
    DoFixBrushes();
  } else if( string_equal_nocase( p, "polygon builder" ) ) {
    DoPolygonsTB();
  } else if( string_equal_nocase( p, "caulk selection" ) ) {
    DoCaulkSelection();
  } else if( string_equal_nocase( p, "tree planter" ) ) {
    DoTreePlanter();
  } else if( string_equal_nocase( p, "plot splines" ) ) {
    DoTrainPathPlot();
  } else if( string_equal_nocase( p, "drop entity" ) ) {
    DoDropEnts();
  } else if( string_equal_nocase( p, "merge patches" ) ) {
    DoMergePatches();
  } else if( string_equal_nocase( p, "split patches" ) ) {
    DoSplitPatch();
  } else if( string_equal_nocase( p, "turn edge" ) ) {
    DoFlipTerrain();
  } else if( string_equal_nocase(p, "reset textures...") ) {
		DoResetTextures();
	} else if( string_equal_nocase(p, "pitomatic") ) {
		DoPitBuilder();
	} else if( string_equal_nocase(p, "vis viewer") ) {
		DoVisAnalyse();
	} else if( string_equal_nocase(p, "about...") ) {
		DoMessageBox(PLUGIN_ABOUT, "About", eMB_OK);
	}
}

#define NUM_TOOLBARBUTTONS 9

unsigned int ToolbarButtonCount( void ) {
	return NUM_TOOLBARBUTTONS;
}

// Load a xpm file and return a pixmap widget.
GtkWidget* new_pixmap (char* filename) {
	GdkPixmap *gdkpixmap;
	GdkBitmap *mask;
	GtkWidget *pixmap;

	g_FuncTable.m_pfnLoadBitmap(filename, (void **)&gdkpixmap, (void **)&mask);	
	pixmap = gtk_pixmap_new (gdkpixmap, mask);

	gdk_pixmap_unref (gdkpixmap);
	gdk_pixmap_unref (mask);

	return pixmap;
} 

class CBobtoolzToolbarButton : public IToolbarButton
{
public:
  virtual const char* getImage() const
  {
    switch( mIndex ) {
      case 0: return "bobtoolz_cleanup.bmp";
      case 1: return "bobtoolz_poly.bmp";
      case 2: return "bobtoolz_caulk.bmp";
      case 3: return "bobtoolz_treeplanter.bmp";
      case 4: return "bobtoolz_trainpathplot.bmp";
      case 5: return "bobtoolz_dropent.bmp";
      case 6: return "bobtoolz_merge.bmp";
      case 7: return "bobtoolz_split.bmp";
      case 8: return "bobtoolz_turnedge.bmp";
    }
    return NULL;
  }
  virtual EType getType() const
  {
    switch( mIndex ) {
      case 3: return eToggleButton;
      default: return eButton;
    }    
  }
  virtual const char* getText() const
  {
    switch( mIndex ) {
      case 0: return "Cleanup";
      case 1: return "Polygons";
      case 2: return "Caulk";
      case 3: return "Tree Planter";
      case 4: return "Plot Splines";
      case 5: return "Drop Entity";
      case 6: return "Merge Patches";
      case 7: return "Split Patches";
      case 8: return "Flip Terrain";
    }
    return NULL;
  }
  virtual const char* getTooltip() const
  {
    switch( mIndex ) {
      case 0: return "Brush Cleanup";
      case 1: return "Polygons";
      case 2: return "Caulk selection";
      case 3: return "Tree Planter";
      case 4: return "Plot Splines";
      case 5: return "Drop Entity";
      case 6: return "Merge Patches";
      case 7: return "Split Patches";
      case 8: return "Flip Terrain";
    }
    return NULL;
  }

  virtual void activate() const
  {
  	LoadLists();

    switch( mIndex ) {
      case 0: DoFixBrushes(); break;
      case 1: DoPolygonsTB(); break;
      case 2: DoCaulkSelection(); break;
      case 3: DoTreePlanter(); break;
      case 4: DoTrainPathPlot(); break;
      case 5: DoDropEnts(); break;
      case 6: DoMergePatches(); break;
      case 7: DoSplitPatch(); break;
      case 8: DoFlipTerrain(); break;
    }
  }

  int mIndex;
};

CBobtoolzToolbarButton g_bobtoolzToolbarButtons[NUM_TOOLBARBUTTONS];

const IToolbarButton* GetToolbarButton(unsigned int index)
{
  g_bobtoolzToolbarButtons[index].mIndex = index;
  return &g_bobtoolzToolbarButtons[index];
}

// =============================================================================
// SYNAPSE

#include "synapse.h"

class CSynapseClientBobtoolz : public CSynapseClient
{
public:
  // CSynapseClient API
  bool RequestAPI(APIDescriptor_t *pAPI);
  const char* GetInfo();
  
  CSynapseClientBobtoolz() { }
  virtual ~CSynapseClientBobtoolz() { }
};

#define BOBTOOLZ_MINOR "bobtoolz"

CSynapseServer* g_pSynapseServer = NULL;
CSynapseClientBobtoolz g_SynapseClient;

extern "C" CSynapseClient* SYNAPSE_DLL_EXPORT Synapse_EnumerateInterfaces (const char *version, CSynapseServer *pServer)
{
  if (strcmp(version, SYNAPSE_VERSION))
  {
    Syn_Printf("ERROR: synapse API version mismatch: should be '" SYNAPSE_VERSION "', got '%s'\n", version);
    return NULL;
  }
  g_pSynapseServer = pServer;
  g_pSynapseServer->IncRef();
  Set_Syn_Printf(g_pSynapseServer->Get_Syn_Printf());
    
  g_SynapseClient.AddAPI(TOOLBAR_MAJOR, BOBTOOLZ_MINOR, sizeof(_QERPlugToolbarTable));
  g_SynapseClient.AddAPI(PLUGIN_MAJOR, BOBTOOLZ_MINOR, sizeof(_QERPluginTable));

  g_SynapseClient.AddAPI(BRUSH_MAJOR, NULL, sizeof(__BRUSHTABLENAME), SYN_REQUIRE, &g_BrushTable);
  g_SynapseClient.AddAPI(PATCH_MAJOR, NULL, sizeof(__PATCHTABLENAME), SYN_REQUIRE, &g_BrushTable);
  g_SynapseClient.AddAPI(SHADERS_MAJOR, "*", sizeof(g_ShadersTable), SYN_REQUIRE, &g_ShadersTable);
  g_SynapseClient.AddAPI(ENTITY_MAJOR, NULL, sizeof(g_EntityTable), SYN_REQUIRE, &g_EntityTable);
  g_SynapseClient.AddAPI(SELECTEDFACE_MAJOR, NULL, sizeof(g_SelectedFaceTable), SYN_REQUIRE, &g_SelectedFaceTable);
  g_SynapseClient.AddAPI(UI_MAJOR, NULL, sizeof(g_MessageTable), SYN_REQUIRE, &g_MessageTable);
  g_SynapseClient.AddAPI(RADIANT_MAJOR, NULL, sizeof(__QERTABLENAME), SYN_REQUIRE, &g_FuncTable);
  g_SynapseClient.AddAPI(QGL_MAJOR, NULL, sizeof(g_QglTable), SYN_REQUIRE, &g_QglTable);

  return &g_SynapseClient;
}

bool CSynapseClientBobtoolz::RequestAPI(APIDescriptor_t *pAPI)
{
  if( !strcmp(pAPI->minor_name, BOBTOOLZ_MINOR) )
  {
    if( !strcmp(pAPI->major_name, PLUGIN_MAJOR) )
    {
      _QERPluginTable* pTable= static_cast<_QERPluginTable*>(pAPI->mpTable);

      pTable->m_pfnQERPlug_Init = QERPlug_Init;
      pTable->m_pfnQERPlug_GetName = QERPlug_GetName;
      pTable->m_pfnQERPlug_GetCommandList = QERPlug_GetCommandList;
      pTable->m_pfnQERPlug_Dispatch = QERPlug_Dispatch;

      return true;
    }
    else if( !strcmp(pAPI->major_name, TOOLBAR_MAJOR) )
    {
      _QERPlugToolbarTable* pTable= static_cast<_QERPlugToolbarTable*>(pAPI->mpTable);

      pTable->m_pfnToolbarButtonCount = &ToolbarButtonCount;
      pTable->m_pfnGetToolbarButton = &GetToolbarButton;

      return true;
    }
  }

  Syn_Printf("ERROR: RequestAPI( '%s' ) not found in '%s'\n", pAPI->major_name, GetInfo());
  return false;
}

#include "version.h"

const char* CSynapseClientBobtoolz::GetInfo()
{
  return "bobToolz module built " __DATE__ " " RADIANT_VERSION;
}

