//---------------------------------------------------------------------
// Simple C++ retained mode library for Verse
// Copyright (c) PDC, KTH
// Written by Camilla Berglund <clb@kth.se>
//---------------------------------------------------------------------

#include <verse.h>

#include <Ample.h>

#include <cstring>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

namespace
{

bool operator != (const VNMRampPoint& first, const VNMRampPoint& second)
{
  return first.pos != second.pos ||
         first.red != second.red ||
	 first.green != second.green ||
	 first.blue != second.blue;
}

bool compareFragments(const VMatFrag& first, const VMatFrag& second, VNMFragmentType type)
{
  switch (type)
  {
    case VN_M_FT_COLOR:
    {
      return first.color.red == second.color.red &&
             first.color.green == second.color.green &&
	     first.color.blue == second.color.blue;
    }

    case VN_M_FT_LIGHT:
    {
      if (first.light.type != second.light.type ||
          first.light.normal_falloff != second.light.normal_falloff ||
	  first.light.brdf != second.light.brdf)
	return false;

      for (unsigned int i = 0;  i < 16;  i++)
      {
	if (first.light.brdf_r[i] != second.light.brdf_r[i] ||
	    first.light.brdf_g[i] != second.light.brdf_g[i] ||
	    first.light.brdf_b[i] != second.light.brdf_b[i])
	  return false;
      }

      return true;
    }

    case VN_M_FT_REFLECTION:
    {
      return first.reflection.normal_falloff == second.reflection.normal_falloff;
    }

    case VN_M_FT_TRANSPARENCY:
    {
      return first.transparency.normal_falloff == second.transparency.normal_falloff &&
             first.transparency.refraction_index == second.transparency.refraction_index;
    }

    case VN_M_FT_VOLUME:
    {
      return first.volume.diffusion == second.volume.diffusion &&
             first.volume.col_r == second.volume.col_r &&
             first.volume.col_g == second.volume.col_g &&
             first.volume.col_b == second.volume.col_b;
    }

    case VN_M_FT_VIEW:
    {
      return true;
    }

    case VN_M_FT_GEOMETRY:
    {
      for (unsigned int i = 0;  i < 16;  i++)
      {
	if (first.geometry.layer_r[i] != second.geometry.layer_r[i] ||
	    first.geometry.layer_g[i] != second.geometry.layer_g[i] ||
	    first.geometry.layer_b[i] != second.geometry.layer_b[i])
	  return false;
      }

      return true;
    }

    case VN_M_FT_TEXTURE:
    {
      for (unsigned int i = 0;  i < 16;  i++)
      {
	if (first.texture.layer_r[i] != second.texture.layer_r[i] ||
	    first.texture.layer_g[i] != second.texture.layer_g[i] ||
	    first.texture.layer_b[i] != second.texture.layer_b[i])
	  return false;
      }

      return first.texture.bitmap == second.texture.bitmap &&
             first.texture.filtered == second.texture.filtered &&
	     first.texture.mapping == second.texture.mapping;
    }

    case VN_M_FT_NOISE:
    {
      return first.noise.type == second.noise.type &&
             first.noise.mapping == second.noise.mapping;
    }

    case VN_M_FT_BLENDER:
    {
      return first.blender.type == second.blender.type &&
             first.blender.data_a == second.blender.data_a &&
             first.blender.data_b == second.blender.data_b &&
	     first.blender.control == second.blender.control;
    }

    case VN_M_FT_CLAMP:
    {
      return first.clamp.min == second.clamp.min &&
             first.clamp.red == second.clamp.red &&
	     first.clamp.green == second.clamp.green &&
	     first.clamp.blue == second.clamp.blue &&
	     first.clamp.data == second.clamp.data;
    }

    case VN_M_FT_MATRIX:
    {
      for (unsigned int i = 0;  i < 16;  i++)
      {
	if (first.matrix.matrix[i] != second.matrix.matrix[i])
	  return false;
      }

      return first.matrix.data == second.matrix.data;
    }

    case VN_M_FT_RAMP:
    {
      if (first.ramp.point_count != second.ramp.point_count)
	return false;

      for (unsigned int i = 0;  i < first.ramp.point_count;  i++)
      {
	if (first.ramp.ramp[i] != second.ramp.ramp[i])
	  return false;
      }

      return first.ramp.type == second.ramp.type &&
             first.ramp.channel == second.ramp.channel &&
	     first.ramp.mapping == second.ramp.mapping;
    }

    case VN_M_FT_ANIMATION:
    {
      return std::strcmp(first.animation.label, second.animation.label) == 0;
    }

    case VN_M_FT_ALTERNATIVE:
    {
      return first.alternative.alt_a == second.alternative.alt_a &&
             first.alternative.alt_b == second.alternative.alt_b;
    }

    case VN_M_FT_OUTPUT:
    {
      return first.output.front == second.output.front &&
             first.output.back == second.output.back &&
	     std::strcmp(first.output.label, second.output.label) == 0;
    }
  }

  return false;
}

}

//---------------------------------------------------------------------

void Fragment::destroy(void)
{
  getNode().getSession().push();
  verse_send_m_fragment_destroy(getNode().getID(), mID);
  getNode().getSession().pop();
}

VNMFragmentID Fragment::getID(void) const
{
  return mID;
}

VNMFragmentType Fragment::getType(void) const
{
  return mType;
}

const VMatFrag& Fragment::getValue(void) const
{
  return mValue;
}

MaterialNode& Fragment::getNode(void) const
{
  return mNode;
}

Fragment::Fragment(VNMFragmentID ID, MaterialNode& node, VNMFragmentType type, const VMatFrag& value):
  mID(ID),
  mNode(node),
  mType(type),
  mValue(value)
{
}

//---------------------------------------------------------------------

void FragmentObserver::onSetType(Fragment& fragment, VNMFragmentType type)
{
}

void FragmentObserver::onSetValue(Fragment& fragment, const VMatFrag& value)
{
}

void FragmentObserver::onDestroy(Fragment& fragment)
{
}

//---------------------------------------------------------------------

void MaterialNode::createFragment(VNMFragmentID ID, VNMFragmentType type, const VMatFrag& value)
{
  getSession().push();
  verse_send_m_fragment_create(getID(), ID, type, &value);
  getSession().pop();
}

Fragment* MaterialNode::getFragmentByID(VNMFragmentID ID)
{
  for (FragmentList::iterator i = mFragments.begin();  i != mFragments.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const Fragment* MaterialNode::getFragmentByID(VNMFragmentID ID) const
{
  for (FragmentList::const_iterator i = mFragments.begin();  i != mFragments.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

Fragment* MaterialNode::getFragmentByIndex(unsigned int index)
{
  return mFragments[index];
}

const Fragment* MaterialNode::getFragmentByIndex(unsigned int index) const
{
  return mFragments[index];
}

uint16 MaterialNode::getFragmentCount(void) const
{
  return mFragments.size();
}

MaterialNode::MaterialNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_MATERIAL, owner, session)
{
}

MaterialNode::~MaterialNode(void)
{
  while (!mFragments.empty())
  {
    delete mFragments.back();
    mFragments.pop_back();
  }
}

void MaterialNode::initialize(void)
{
  verse_callback_set((void*) verse_send_m_fragment_create,
                     (void*) receiveFragmentCreate,
                     NULL);
  verse_callback_set((void*) verse_send_m_fragment_destroy,
                     (void*) receiveFragmentDestroy,
                     NULL);
}

void MaterialNode::receiveFragmentCreate(void* user,
                                         VNodeID nodeID,
                                         VNMFragmentID fragmentID,
                                         VNMFragmentType type,
                                         const VMatFrag* value)
{
  Session* session = Session::getCurrent();

  MaterialNode* node = dynamic_cast<MaterialNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  Fragment* fragment = node->getFragmentByID(fragmentID);
  if (fragment)
  {
    if (type != fragment->mType)
    {
      const Fragment::ObserverList& observers = fragment->getObservers();
      for (Fragment::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
        (*i)->onSetType(*fragment, type);

      fragment->mType = type;
      fragment->mValue = *value;
      fragment->updateStructureVersion();

      //for (Fragment::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      //  (*i)->postSetType(*fragment, type);
    }
    else
    {
      const Fragment::ObserverList& observers = fragment->getObservers();
      for (Fragment::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
        (*i)->onSetValue(*fragment, *value);

      fragment->mValue = *value;
      fragment->updateDataVersion();

      //for (Fragment::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      //  (*i)->postFragChange(*fragment, frag);
    }
  }
  else
  {
    fragment = new Fragment(fragmentID, *node, type, *value);

	// Notify node observers.
    const MaterialNode::ObserverList& nObservers = node->getObservers();
    for (MaterialNode::ObserverList::const_iterator i = nObservers.begin();  i != nObservers.end();  i++)
    {
      if (MaterialNodeObserver* observer = dynamic_cast<MaterialNodeObserver*>(*i))
        observer->onCreateFragment(*node, *fragment);
    }

    node->mFragments.push_back(fragment);
    node->updateStructureVersion();

    //for (MaterialNode::ObserverList::const_iterator i = nObservers.begin();  i != nObservers.end();  i++)
    //{
    //  if (MaterialNodeObserver* observer = dynamic_cast<MaterialNodeObserver*>(*i))
    //    observer->postCreateFragment(*node, *fragment);
    //}
  }
}

void MaterialNode::receiveFragmentDestroy(void* user,
                                          VNodeID nodeID,
                                          VNMFragmentID fragmentID)
{
  Session* session = Session::getCurrent();

  MaterialNode* node = dynamic_cast<MaterialNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  //Fragment* fragment = node->getFragmentByID(fragmentID);
  //if (fragment)
  for (MaterialNode::FragmentList::iterator f = node->mFragments.begin();  f != node->mFragments.end();  f++)
  {
    if ((*f)->getID() == fragmentID)
    {
      // Notify fragment observers.
      const Fragment::ObserverList& fObservers = (*f)->getObservers();
      for (Fragment::ObserverList::const_iterator i = fObservers.begin();  i != fObservers.end();  i++)
      {
        if (FragmentObserver* observer = dynamic_cast<FragmentObserver*>(*i))
	  observer->onDestroy(*(*f));
      }

      // Notify node observers.
      const MaterialNode::ObserverList& nObservers = node->getObservers();
      for (MaterialNode::ObserverList::const_iterator i = nObservers.begin();  i != nObservers.end();  i++)
      {
	if (MaterialNodeObserver* observer = dynamic_cast<MaterialNodeObserver*>(*i))
          observer->onDestroyFragment(*node, *(*f));
      }

      delete *f;

      node->mFragments.erase(f);
      node->updateStructureVersion();

      //delete *f; SLAS::TMP
      //    Fragment* frag = *f;

      // SLAS::TMP
      /*
      for (Fragment::ObserverList::const_iterator i = fObservers.begin();  i != fObservers.end();  i++)
      {
        if (FragmentObserver* observer = dynamic_cast<FragmentObserver*>(*i))
          observer->postDestroy(*frag);
      }

      for (MaterialNode::ObserverList::const_iterator i = nObservers.begin();  i != nObservers.end();  i++)
      {
	      if (MaterialNodeObserver* observer = dynamic_cast<MaterialNodeObserver*>(*i))
            observer->postDestroyFragment(*node, *frag);
      }
      */

      //delete frag; // SLAS::TMP

      break;
    }
  }
}

//---------------------------------------------------------------------

void MaterialNodeObserver::onCreateFragment(MaterialNode& node, Fragment& fragment)
{
}

void MaterialNodeObserver::onDestroyFragment(MaterialNode& node, Fragment& fragment)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

