/*****************************************************************************
 * DynaMechs: A Multibody Dynamic Simulation Library
 *
 * Copyright (C) 1994-2001  Scott McMillan   All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************
 *     File: dmLoadfile_dm21.cpp
 *   Author: Scott McMillan
 *  Created:
 *  Summary:
 *****************************************************************************/

#include "../dm/dm.h"

#ifdef WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "../dm/dmObject.hpp"
#include "../dm/dmSystem.hpp"
#include "../dm/dmArticulation.hpp"
#include "../dm/dmLink.hpp"
#include "../dm/dmZScrewTxLink.hpp"
#include "../dm/dmMDHLink.hpp"
#include "../dm/dmRevoluteLink.hpp"
#include "../dm/dmPrismaticLink.hpp"
#include "../dm/dmSphericalLink.hpp"
#include "../dm/dmMobileBaseLink.hpp"
#include "../dm/dmActuator.hpp"
#include "../dm/dmRevDCMotor.hpp"

#include "../dm/dmContactModel.hpp"

#include "dmLoadFile.h"
#include "glLoadModels.h"

int line_num21;

int L_num_graphics_models21 = 0;
GLuint *L_solid_models21;

const int NAME_SIZE = 256;
char object_name21[NAME_SIZE];

//----------------------------------------------------------------------------
GLuint *dmGLGetSolidModelIndex21(int graphics_model_index)
{
   GLuint *dlist_index = new GLuint;
   *dlist_index = 0;

   if ((graphics_model_index < L_num_graphics_models21) &&
       (graphics_model_index >= 0))
      *dlist_index = L_solid_models21[graphics_model_index];

   return dlist_index;
}

//----------------------------------------------------------------------------
GLuint *getSolidModelIndex21(ifstream &cfg_ptr)
{
   int graphics_model_index;

   readConfigParameterLabel(cfg_ptr, "Graphics_Model_Index");
   cfg_ptr >> graphics_model_index;

   return dmGLGetSolidModelIndex21(graphics_model_index);
}

//----------------------------------------------------------------------------
void dmGLLoadModels21(int num_models, char **filename)
{
   // get solid object (rigid body) models.
   L_num_graphics_models21 = num_models;
   L_solid_models21 = new GLuint[L_num_graphics_models21];

   if (L_num_graphics_models21 > 0)
   {
      for (int i=0; i<L_num_graphics_models21; i++)
      {
         //cout << "Graphics Model file #" << i << ": " << filename[i]
         //     << endl << flush;
         if (strstr(filename[i], ".scm"))
            L_solid_models21[i] = dmGLLoadFile_scm(filename[i]);
         else if (strstr(filename[i], ".xan"))
            L_solid_models21[i] = dmGLLoadFile_xan(filename[i]);
         else if (strstr(filename[i], ".cmb"))
            L_solid_models21[i] = dmGLLoadFile_cmb(filename[i]);
         else
            cerr << "Error: unknown model format for " << filename[i] << endl;
      }
   }
}

//----------------------------------------------------------------------------
void loadModels21(ifstream &cfg_ptr)
{
   int num_graphics_models;

   // get solid object (rigid body) models.
   readConfigParameterLabel(cfg_ptr,"Number_Graphics_Models");
   cfg_ptr >> num_graphics_models;

   if (num_graphics_models > 0)
   {
      int i;
      char **filename;
      filename = new char*[num_graphics_models];

      for (i=0; i<num_graphics_models; i++)
      {
         filename[i] = new char[FILENAME_SIZE];
         readFilename(cfg_ptr, filename[i]);
      }

      dmGLLoadModels21(num_graphics_models, filename);

      for (i=0; i<num_graphics_models; i++)
      {
         delete filename[i];
      }
      delete [] filename;
   }
}


//----------------------------------------------------------------------------
bool getName21(ifstream &cfg_ptr, char *name)
{
   readConfigParameterLabel(cfg_ptr, "Name");

   if (cfg_ptr.getline(name, NAME_SIZE, '\042'))
   {
      if (cfg_ptr.getline(name, NAME_SIZE, '\042'))
      {
         if (strlen(name) > 0)
         {
            //cerr << "Got name: " << name << endl;
            return true;
         }
      }
   }
   return false;
}

//----------------------------------------------------------------------------
void setContactParameters21(dmRigidBody *body, ifstream &cfg_ptr)
{
   unsigned int num_points;
   readConfigParameterLabel(cfg_ptr, "Number_of_Contact_Points");
   cfg_ptr >> num_points;

   if (num_points > 0)
   {
      readConfigParameterLabel(cfg_ptr, "Contact_Locations");

      CartesianVector *pos = new CartesianVector[num_points];

      for (unsigned int i=0; i<num_points; i++)
      {
         cfg_ptr >> pos[i][0]
                 >> pos[i][1]
                 >> pos[i][2];
      }

      dmContactModel *contact_model = new dmContactModel();
      contact_model->setContactPoints(num_points, pos);
      //contact_model->setEnvironment(&G_environment);
      body->addForce(contact_model);

      delete [] pos;
   }
}

//----------------------------------------------------------------------------
void setRigidBodyParameters21(dmRigidBody *body, ifstream &cfg_ptr)
{
   register int i;

// dynamic properties:
   Float mass;
   CartesianTensor I_bar;
   CartesianVector cg_pos;

   readConfigParameterLabel(cfg_ptr, "Mass");
   cfg_ptr >> mass;
   readConfigParameterLabel(cfg_ptr, "Inertia");
   for (i = 0; i < 3; i++)
   {
      cfg_ptr >> I_bar[i][0] >> I_bar[i][1] >> I_bar[i][2];
   }
   readConfigParameterLabel(cfg_ptr, "Center_of_Gravity");
   cfg_ptr >> cg_pos[0] >> cg_pos[1] >> cg_pos[2];

   body->setInertiaParameters(mass, I_bar, cg_pos);

#ifdef DM_HYDRODYNAMICS
// hydrodynamic properties:
   Float volume;
   SpatialTensor I_added_mass;
   CartesianVector cb_pos;
   int drag_axis;
   Float cyl_min;
   Float cyl_max;
   Float cyl_radius;
   Float C_d;

   readConfigParameterLabel(cfg_ptr, "Volume");
   cfg_ptr >> volume;

   readConfigParameterLabel(cfg_ptr, "Added_Mass");
   for (i = 0; i < 6; i++)
      for (unsigned int j = 0; j < 6; j++)
         cfg_ptr >> I_added_mass[i][j];

   readConfigParameterLabel(cfg_ptr, "Center_of_Buoyancy");
   cfg_ptr >> cb_pos[0] >> cb_pos[1] >> cb_pos[2];

// drag parameters
   readConfigParameterLabel(cfg_ptr, "Drag_Axis");
   cfg_ptr >> drag_axis;

   readConfigParameterLabel(cfg_ptr, "Cylinder_Bounds");
   cfg_ptr >> cyl_min >> cyl_max;

   readConfigParameterLabel(cfg_ptr, "Cylinder_Radius");
   cfg_ptr >> cyl_radius;

   readConfigParameterLabel(cfg_ptr, "Drag_Coefficient");
   cfg_ptr >> C_d;

   body->setHydrodynamicParameters(volume, I_added_mass, cb_pos,
                                   drag_axis,
                                   cyl_min, cyl_max, cyl_radius, C_d);
#endif

   setContactParameters21(body, cfg_ptr);
}


//----------------------------------------------------------------------------
void setJointFriction21(dmLink *link, ifstream &cfg_ptr)
{
   Float joint_friction;

   readConfigParameterLabel(cfg_ptr, "Joint_Friction");
   cfg_ptr >> joint_friction;

   link->setJointFriction(joint_friction);
}

//----------------------------------------------------------------------------
void setMDHParameters21(dmMDHLink *link, ifstream &cfg_ptr)
{
   Float a, alpha, d, theta;
   Float q, qd;

   readConfigParameterLabel(cfg_ptr, "MDH_Parameters");
   cfg_ptr >> a >> alpha >> d >> theta;

   link->setMDHParameters(a, alpha, d, theta);
   link->getState(&q, &qd);  // either d or theta could be q this retrieves it

   readConfigParameterLabel(cfg_ptr, "Initial_Joint_Velocity");
   cfg_ptr >> qd;

   link->setState(&q, &qd);  // essentially sets the joint velocity

   Float min, max, spring, damper;

   readConfigParameterLabel(cfg_ptr, "Joint_Limits");
   cfg_ptr >> min >> max;
   readConfigParameterLabel(cfg_ptr, "Joint_Limit_Spring_Constant");
   cfg_ptr >> spring;
   readConfigParameterLabel(cfg_ptr, "Joint_Limit_Damper_Constant");
   cfg_ptr >> damper;

   link->setJointLimits(min, max, spring, damper);
}

//----------------------------------------------------------------------------
void setRevDCMotorParameters21(dmRevDCMotor *actuator, ifstream &cfg_ptr)
{
   Float torque_constant;
   Float back_EMF_constant;
   Float armature_resistance;
   Float rotor_inertia;
   Float coulomb_friction_constant;
   Float viscous_friction_constant;
   Float max_brush_drop;
   Float half_drop_value;

   readConfigParameterLabel(cfg_ptr, "Motor_Torque_Constant");
   cfg_ptr >> torque_constant;
   readConfigParameterLabel(cfg_ptr, "Motor_BackEMF_Constant");
   cfg_ptr >> back_EMF_constant;
   readConfigParameterLabel(cfg_ptr, "Motor_Armature_Resistance");
   cfg_ptr >> armature_resistance;
   readConfigParameterLabel(cfg_ptr, "Motor_Inertia");
   cfg_ptr >> rotor_inertia;

   readConfigParameterLabel(cfg_ptr, "Motor_Coulomb_Friction_Constant");
   cfg_ptr >> coulomb_friction_constant;
   readConfigParameterLabel(cfg_ptr, "Motor_Viscous_Friction_Constant");
   cfg_ptr >> viscous_friction_constant;

   readConfigParameterLabel(cfg_ptr, "Motor_Max_Brush_Drop");
   cfg_ptr >> max_brush_drop;
   readConfigParameterLabel(cfg_ptr, "Motor_Half_Drop_Value");
   cfg_ptr >> half_drop_value;

   actuator->setParameters(torque_constant,
                           back_EMF_constant,
                           armature_resistance,
                           rotor_inertia,
                           coulomb_friction_constant,
                           viscous_friction_constant,
                           max_brush_drop,
                           half_drop_value);
}

//----------------------------------------------------------------------------
void setActuator21(dmRevoluteLink *link, ifstream &cfg_ptr)
{
   int actuator_type;

   readConfigParameterLabel(cfg_ptr, "Actuator_Type");
   cfg_ptr >> actuator_type;

   if (actuator_type == NOMOTOR)
   {
      setJointFriction21(link, cfg_ptr);
   }
   else if (actuator_type == DCMOTOR)
   {
      dmRevDCMotor *actuator = new dmRevDCMotor();
      setRevDCMotorParameters21(actuator, cfg_ptr);
      link->setActuator(actuator);
   }
   else
   {
      cerr << "Error: invalid Actuator_Type\n";
      exit(3);
   }
}

//----------------------------------------------------------------------------
void setSphericalLinkParameters21(dmSphericalLink *link, ifstream &cfg_ptr)
{
   // Joint info:
   CartesianVector p;
   readConfigParameterLabel(cfg_ptr, "Position_From_Inboard_Link");
   cfg_ptr >> p[0] >> p[1] >> p[2];
   link->setJointOffset(p);

   EulerAngles ang;
   readConfigParameterLabel(cfg_ptr, "Initial_Joint_Angles");
   cfg_ptr >> ang[0] >> ang[1] >> ang[2];

   Float qd[dmSphericalLink::NUM_DOFS];
   readConfigParameterLabel(cfg_ptr, "Initial_Angular_Velocity");
   cfg_ptr >> qd[0] >> qd[1] >> qd[2];
   link->setState(ang, qd);

   Float joint_limit[dmSphericalLink::NUM_DOFS];
   Float spring, damper;
   readConfigParameterLabel(cfg_ptr, "Axes_Limits");
   cfg_ptr >> joint_limit[0] >> joint_limit[1] >> joint_limit[2];
   readConfigParameterLabel(cfg_ptr, "Joint_Limit_Spring_Constant");
   cfg_ptr >> spring;
   readConfigParameterLabel(cfg_ptr, "Joint_Limit_Damper_Constant");
   cfg_ptr >> damper;
   link->setJointLimits(joint_limit, spring, damper);

   setJointFriction21(link, cfg_ptr);
}

//----------------------------------------------------------------------------
void setDynamicRefMemParameters21(dmMobileBaseLink *ref, ifstream &cfg_ptr)
{
   Float q[7];
   SpatialVector vel;

   readConfigParameterLabel(cfg_ptr, "Position");
   cfg_ptr >> q[4] >> q[5] >> q[6];
   readConfigParameterLabel(cfg_ptr, "Orientation_Quat");
   cfg_ptr >> q[0] >> q[1] >> q[2] >> q[3];

   readConfigParameterLabel(cfg_ptr, "Velocity");
   for (int i = 0; i < 6; i++)
   {
      cfg_ptr >> vel[i];
   }

   ref->setState(q, vel);
}

//----------------------------------------------------------------------------
void setStaticRefMemParameters21(dmSystem *robot, ifstream &cfg_ptr)
{
   CartesianVector pos;
   dmQuaternion quat;

   readConfigParameterLabel(cfg_ptr, "Position");
   cfg_ptr >> pos[0] >> pos[1] >> pos[2];
   readConfigParameterLabel(cfg_ptr, "Orientation_Quat");
   cfg_ptr >> quat[0] >> quat[1] >> quat[2] >> quat[3];

   robot->setRefSystem(quat, pos);
}

//----------------------------------------------------------------------------
void parseBranch21(ifstream &cfg_ptr,
                   dmArticulation *articulation, dmLink *parent)
{
   char *tok;

   dmLink* last_link = parent;

   for (;;)
   {
      // get next token
      tok = getNextToken(cfg_ptr, line_num21);

      // allocate a link and pass stream reference
      if (strcmp(tok, "Articulation") == 0)
      {
         parseToBlockBegin(cfg_ptr, line_num21);

         char art_name[NAME_SIZE];
         getName21(cfg_ptr, art_name);

         parseBranch21(cfg_ptr, articulation, last_link);
      }
      else if (strcmp(tok, "Branch") == 0)
      {
         parseToBlockBegin(cfg_ptr, line_num21);
         parseBranch21(cfg_ptr, articulation, last_link);
      }
      else if (strcmp(tok, "RevoluteLink") == 0)
      {
         parseToBlockBegin(cfg_ptr, line_num21);

         dmRevoluteLink *link = new dmRevoluteLink();

         if (getName21(cfg_ptr, object_name21))
         {
            link->setName(object_name21);
         }

         GLuint *dlist_index = getSolidModelIndex21(cfg_ptr);
         link->setUserData((void *) dlist_index);

         setRigidBodyParameters21(link, cfg_ptr);
         setMDHParameters21(link, cfg_ptr);
         setActuator21(link, cfg_ptr);

         parseToBlockEnd(cfg_ptr, line_num21);

         articulation->addLink(link, last_link);
         last_link = (dmLink *) link;
      }
      else if (strcmp(tok, "PrismaticLink") == 0)
      {
         parseToBlockBegin(cfg_ptr, line_num21);

         dmPrismaticLink *link = new dmPrismaticLink();

         if (getName21(cfg_ptr, object_name21))
         {
            link->setName(object_name21);
         }

         GLuint *dlist_index = getSolidModelIndex21(cfg_ptr);
         link->setUserData((void *) dlist_index);

         setRigidBodyParameters21(link, cfg_ptr);
         setMDHParameters21(link, cfg_ptr);
         setJointFriction21(link, cfg_ptr);

         parseToBlockEnd(cfg_ptr, line_num21);

         articulation->addLink(link, last_link);
         last_link = link;
      }
      else if (strcmp(tok, "SphericalLink") == 0)
      {
         parseToBlockBegin(cfg_ptr, line_num21);

         dmSphericalLink *link = new dmSphericalLink();

         if (getName21(cfg_ptr, object_name21))
         {
            link->setName(object_name21);
         }

         GLuint *dlist_index = getSolidModelIndex21(cfg_ptr);
         link->setUserData((void *) dlist_index);

         setRigidBodyParameters21(link, cfg_ptr);
         setSphericalLinkParameters21(link, cfg_ptr);

         parseToBlockEnd(cfg_ptr, line_num21);

         articulation->addLink(link, last_link);
         last_link = link;
      }
      else if ((strcmp(tok, "ZScrewTxLink") == 0) ||
               (strcmp(tok, "ZScrewLinkTx") == 0))    // latter is deprecated
      {
         parseToBlockBegin(cfg_ptr, line_num21);

         int got_name = getName21(cfg_ptr, object_name21);

         Float d, theta;

         // Get Inboard to Chain base transformation info.
         readConfigParameterLabel(cfg_ptr, "ZScrew_Parameters");
         cfg_ptr >> d >> theta;

         dmZScrewTxLink *link = new dmZScrewTxLink(d, theta);
         if (got_name)
         {
            link->setName(object_name21);
         }
         parseToBlockEnd(cfg_ptr, line_num21);

         articulation->addLink(link, last_link);
         last_link = link;
      }
      else if (tok[0] == BLOCK_END_CHAR)
      {
         break;
      }
      else
      {
         cerr << "Error: Invalid Joint_Type: " << tok << endl;
         exit(3);
      }
   }
}

//----------------------------------------------------------------------------
dmSystem *loadSystem21(ifstream &cfg_ptr)
{
   dmLink *parent_link = NULL;

   dmArticulation *robot = new dmArticulation();
   if (robot == NULL)
   {
      cerr << "dmLoadFile_dm21::loadSystem21 error: "
           << "unable to allocate dmArticulation" << endl;
      cfg_ptr.close();
      exit(4);
   }

   char *tok;
   parseToBlockBegin(cfg_ptr, line_num21);

   // NEW label the system object
   char sys_name[NAME_SIZE];
   if (getName21(cfg_ptr, sys_name))
   {
      robot->setName(sys_name);
   }

   // ============= Get RefMember info. =================
   do
   {
      tok = getNextToken(cfg_ptr, line_num21);

   } while ((strcmp(tok, "StaticRefMember")  != 0) &&
            (strcmp(tok, "DynamicRefMember") != 0));

   parseToBlockBegin(cfg_ptr, line_num21);
   bool got_ref_name = getName21(cfg_ptr, object_name21);
   GLuint *dlist_index = getSolidModelIndex21(cfg_ptr);

   // dmStaticRefMember corresponds to the 3.0 Ref System within dmSystem now
   if (strcmp(tok, "StaticRefMember") == 0)
   {
      // ignoring the StaticRefMember name (using System name)

      robot->setUserData((void *) dlist_index); // not sure anything can be
                                                // done about this graphics
                                                // model
      setStaticRefMemParameters21(robot, cfg_ptr);
   }

   // dmDynamicRefMember corresponds to the 3.0 MobileBaseLink now
   else if (strcmp(tok, "DynamicRefMember") == 0)
   {
      dmMobileBaseLink *dref = new dmMobileBaseLink();
      if (dref != NULL)
      {
         if (got_ref_name)
         {
            dref->setName(object_name21);
         }
         dref->setUserData((void *) dlist_index);
         setRigidBodyParameters21(dref, cfg_ptr);
         setDynamicRefMemParameters21(dref, cfg_ptr);

         robot->addLink(dref, NULL);
         parent_link = dref;
      }
      else
      {
         cerr << "dmLoadFile_dm21::loadSystem error: unable to allocate "
              << "dmDynamicRefMember" << endl;
         cfg_ptr.close();
         exit(1);
      }
   }

   else
   {
      cerr << "dmLoadFile_dm21::loadSystem error: unknown RefMember type: "
           << tok << endl;
      exit(3);
   }

   parseToBlockEnd(cfg_ptr, line_num21);

   //========================================================================
   parseBranch21(cfg_ptr, robot, parent_link);
   //========================================================================

   return robot;
}


//----------------------------------------------------------------------------
dmSystem *dmLoadFile_dm21(ifstream &cfg_ptr)
{
   line_num21 = 1;
   bool models_flag = true;
   bool system_flag = true;

   dmSystem *robot = NULL;

   do
   {
      char *tok = getNextToken(cfg_ptr, line_num21);

      if (strcmp(tok, "Graphics_Models") == 0)
      {
         parseToBlockBegin(cfg_ptr, line_num21);
         loadModels21(cfg_ptr);
         models_flag = false;
         parseToBlockEnd(cfg_ptr, line_num21);
      }
      else if (strcmp(tok, "System") == 0)
      {
         robot = loadSystem21(cfg_ptr);
         system_flag = false;
      }
      else
      {
         cerr << "dmLoadFile_dm21 error: unknown token on line " << line_num21
              << ": " << tok << endl;
      }
   } while (models_flag || system_flag);

   return robot;
}
