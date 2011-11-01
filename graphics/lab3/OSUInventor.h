//  OSU Inventor classes
// Modified by Benjamin Schroeder (Thanks)

#ifndef _OSU_INVENTOR_H_
#define _OSU_INVENTOR_H_

#include <Inventor/SbPList.h>
#include <Inventor/SoType.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCamera.h>

class TextureType {
    int typeNum;
};

class OSUObjectData {

 public:
   OSUObjectData();                        // constructor
   ~OSUObjectData();                       // destructor
   bool Check() const;                     // return true if data is OK

   // basic information
   SoNode  *shape;                         // The actual OpenInventor shape
   SoTransform *transformation;            // object to world transformation
   SoMaterial  *material;                  // object material

   // more information
   SoNode  *points;      // SoCoordinate2, SoCoordinate3 or SoCoordinate4
   SoTextureCoordinate2  *texture_points;  // texture points
   SoTexture2 *texture;  // texture mapping node, for 2D textures.
   TextureType *texture_type;              // id for parametric textures
   SoComplexity *complexity; 
   SoDrawStyle  *draw_style;
   SoNormal *normals;                      // stored normals.
};

class OSUInventorScene {

 public:
   OSUInventorScene(char *filename);

   SbPList Objects;   // list of OSUObjectData
   SbPList Lights;    // list of SoLights
   SoCamera *Camera;  // scene camera
};

#endif
