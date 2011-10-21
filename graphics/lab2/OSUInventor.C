//  OSU Inventor classes
// Modified by Benjamin Schroeder, OSU

// general includes
#include <iostream>

// OpenInventor includes
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoLists.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoTextureImageElement.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoVertexShape.h>
#include <Inventor/nodes/SoNonIndexedShape.h>

#include "OSUInventor.h"
using namespace std;
// #define DEBUG 0

//
//	Write a SbMatrix object (ascii)
//
ostream & operator << (ostream & out, const SbMatrix & mat)
{
	static const char space[] = { " " };
	for (int row = 0; row < 4; row++) {
		const float * vec = &mat[row][0];
		out << vec[0] << space << vec[1]
			<< space << vec[2] << space << vec[3] << "\n";
	}
	return out;
}

SoSeparator *ReadScene(const char *Dir, const char *filename) {
   FILE *filePtr = NULL;
   SoSeparator *root;
   SoInput in;

   in.addDirectoryLast(Dir);

   if (!in.openFile(filename)) {
       cerr << "Error opening file " << filename << " from Directory " << Dir 
	    << endl;
       exit(1);
   }
   
   root = SoDB::readAll(&in);
   if (root == NULL)
       cerr << "Error reading file " << filename << " from Directory " << Dir 
	    << endl;
   else {
#ifdef DEBUG
       cerr << "Scene (" << filename << ") read!\n";
#endif
       root->ref();
   }
   in.closeFile();
   return root;
}

SoNode *ReadString(char* string) {
   SoNode *node;
   SoInput in;
   in.setBuffer(string, strlen(string));
   node = SoDB::readAll(&in);
   node->ref();

   return node;
}


SoSeparator *ReadScene(const char *filename) {
   FILE *filePtr = NULL;
   SoSeparator *root;
   SoInput in;

   if (!in.openFile(filename)) {
       cerr << "Error opening file " << filename << endl;
       exit(1);
   }

   root = SoDB::readAll(&in);

   if (root == NULL)
       cerr << "Error reading file " << filename << endl;
   else {
#ifdef DEBUG
       cerr << "Scene (" << filename << ") read!\n";
#endif
       root->ref();
   }
   in.closeFile();
   return root;
}

static SoCallbackAction::Response
processNodesCB(void *userData, SoCallbackAction *ca, const SoNode *node)
{
#ifdef DEBUG
    cerr << "The node is of type " 
	 << node->getTypeId().getName().getString() << endl;
#endif

    if (node->isOfType(SoShape::getClassTypeId())) {
	SbPList *data_list = (SbPList *) userData;

	OSUObjectData *data = new OSUObjectData;
	data_list->append((void *) data);
	data->shape = node->copy();
	data->shape->ref();
	SoVertexShape* vertShape = (SoVertexShape *)data->shape;

	SoState* state = ca->getState();
	SoLazyElement* le = SoLazyElement::getInstance(state);
	le->print(stderr);

	int sIndex = 0;
	if (node->isOfType(SoVertexShape::getClassTypeId())) {
#ifdef DEBUG
	    cerr << "     Found a Vertex Shape!\n";
#endif
	    const SoCoordinateElement* ce;
	    ce = SoCoordinateElement::getInstance(state);

	    //  If we don't start at 0 then why copy those vertices.
	    //  We could also check to see just how many points we use and 
	    //  remove those as well, but that will take a bit more work.

	    if (vertShape->isOfType(SoNonIndexedShape::getClassTypeId())){
		sIndex = ((SoNonIndexedShape*)vertShape)->startIndex.getValue();
		((SoNonIndexedShape*)vertShape)->startIndex.setValue(0);

	    }
	    if (ce->is3D() ){   // It's 3D
#ifdef DEBUG
		cerr << "  There are " << ce->getNum() << " 3D coords\n";
#endif
		SoCoordinate3 *Ps = new SoCoordinate3;
		Ps->point.setValues(0, ce->getNum() - sIndex,&ce->get3(sIndex));
		Ps->ref();
		data->points = (SoNode *) Ps;
	    } else {  // It's 4D
#ifdef DEBUG
		cerr << "  There are " << ce->getNum() << " 4D coords\n";
#endif
		SoCoordinate4 *Ps = new SoCoordinate4;
		Ps->point.setValues(0, ce->getNum() - sIndex,&ce->get4(sIndex));
		Ps->ref();
		data->points = (SoNode *) Ps;
	    }
	    const SoNormalElement* ne = SoNormalElement::getInstance(state);
	    SoNormalBinding::Binding nb = 
		(SoNormalBinding::Binding)SoNormalBindingElement::get(state);
	    int startNrmIndex = 0;
	    if (ne != NULL && ne->getNum()>0){
		// vp->normalBinding.setValue(nb);
	    
		if (nb == SoNormalBinding::PER_VERTEX){
		    startNrmIndex = sIndex;
		}
		data->normals = new SoNormal;
		data->normals->ref();
		data->normals->vector.setValues(0, (ne->getNum())-startNrmIndex, 
		                               &ne->get(startNrmIndex));    
	    }
	}  // End vertex shapes

	// If there are texture coordinates in the state,
	//put them into the vertex property node.
     
    
	int startTxtIndex = 0;
	SoTextureCoordinateBinding::Binding tcb = 
	    (SoTextureCoordinateBinding::Binding)
		SoTextureCoordinateBindingElement::get(state);
	if (tcb == SoTextureCoordinateBinding::PER_VERTEX)
	    startTxtIndex = sIndex;
	const SoTextureCoordinateElement* tce = 
	    SoTextureCoordinateElement::getInstance(state);
	if (tce->getType() == SoTextureCoordinateElement::EXPLICIT &&
	    (tce->getNum() > 0 )){
	    data->texture_points = new SoTextureCoordinate2;
	    data->texture_points->ref();
	    data->texture_points->point.setValues(0, tce->getNum() - startTxtIndex, 
	                          	  &tce->get2(startTxtIndex));
	    // vp->texCoord.setValues(0, tce->getNum() - startTxtIndex, 
	//	&tce->get2(startTxtIndex));
	}
	SbVec2s size;
	int numcomponents, wrapS, wrapT, model;
	SbColor blendColor(0,0,0);

	const unsigned char *timage = SoTextureImageElement::get(state, size, numcomponents, wrapS, wrapT, model, blendColor);

	if (timage) {
	    data->texture = new SoTexture2;
	    data->texture->ref();
	    // data->texture->filename.setValue(tfilename);
	    // data->texture->image.setValue(size, numcomponents, timage);
	    timage = ca->getTextureImage(size, numcomponents);
	    data->texture->image.setValue(size, numcomponents, timage);
	    data->texture->wrapS = wrapS;
	    data->texture->wrapT = wrapT;
	    data->texture->model = model;
	    data->texture->blendColor.setValue(blendColor);
	}
 
        data->complexity = new SoComplexity;
	data->complexity->ref();
	data->complexity->value = ca->getComplexity();
	data->complexity->type = ca->getComplexityType();

	data->draw_style = new SoDrawStyle;
	data->draw_style->ref();
	data->draw_style->style = ca->getDrawStyle();
	data->draw_style->pointSize = ca->getPointSize();
	data->draw_style->lineWidth = ca->getLineWidth();
	data->draw_style->linePattern = ca->getLinePattern();


#ifdef DEBUG
	cerr << "The model matrix is " << ca->getModelMatrix() << endl;
#endif
	SbMatrix const xm = ca->getModelMatrix();

	data->transformation = new SoTransform;
	data->transformation->ref();
	data->transformation->setMatrix(xm);

	SoMaterial *material = new SoMaterial;
	material->ref();
	material->ambientColor = SoLazyElement::getAmbient(state);
        material->diffuseColor = SoLazyElement::getDiffuse(state, 0);
        material->specularColor = SoLazyElement::getSpecular(state);
        material->emissiveColor = SoLazyElement::getEmissive(state);
        material->shininess = SoLazyElement::getShininess(state);
        material->transparency = SoLazyElement::getTransparency(state, 0);
	data->material = material;
    }

    return SoCallbackAction::CONTINUE;
}

OSUObjectData::OSUObjectData() {
   points = NULL;
   shape = NULL;
   transformation = NULL;
   material = NULL;
   texture_points = NULL;
   texture = NULL;
   texture_type = NULL;
   normals = NULL;
   complexity = NULL; 
   draw_style = NULL;
}

OSUObjectData::~OSUObjectData() {
   if (points)
       points->unref();
   if (shape)
       shape->unref();
   if (transformation)
       transformation->unref();
   if (material)
       material->unref();
   if (texture_points)
       texture_points->unref();
   if (texture)
       texture->unref();
   if (texture_type)
       delete texture_type;
   if (normals)
       normals->unref();
   if (complexity)
       complexity->unref();
   if (draw_style)
       draw_style->unref();
}

bool OSUObjectData::Check() const
// return true if data is OK
{
  if (shape == NULL)
    return(false);

  if (transformation == NULL)
    return(false);

  if (material == NULL)
    return(false);

  if (draw_style == NULL)
    return(false);

  return(true); //BUG fixed by Han-Wei Shen, 8/23/2007; not all system will return  true by default
                // so this line is added. 
}

OSUInventorScene::OSUInventorScene(char *filename) {

   SoSeparator *root = ReadScene(filename);
   int numNodes = 0;

   SoCallbackAction ca;
   SoSearchAction SA;
   SA.setType(SoLight::getClassTypeId(), TRUE);
   SA.setInterest(SoSearchAction::ALL);
   SA.apply(root);
   SoPathList &paths = SA.getPaths();
#ifdef DEBUG
   cerr << "There are " << paths.getLength() << " lights " << endl;
#endif
   int i;
   for (i = 0; i < paths.getLength(); i++) {
       Lights.append(paths[i]->getTail()->copy());
   }

   SA.setType(SoCamera::getClassTypeId(), TRUE);
   SA.setInterest(SoSearchAction::FIRST);
   SA.apply(root);
   if (SA.getPath()) {
       Camera = (SoCamera *)SA.getPath()->getTail()->copy();
#ifdef DEBUG
       cerr << "Found a camera!\n";
#endif
   } else
       Camera = NULL;

   ca.addPreCallback(SoNode::getClassTypeId(), processNodesCB, (void *)
       &Objects);
   ca.apply(root);
   // Now lets find the lights and camera!
   //

#ifdef DEBUG
   cerr << "There are " << Objects.getLength() << " shape objects left!\n";
#endif
}
