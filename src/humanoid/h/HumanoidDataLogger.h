/*
 *  humanoidDataLogging.h
 *  DynaMechs
 *
 *  Created by Patrick Wensing on 6/25/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __HUMANOID_DATA_LOGGER_H__
#define __HUMANOID_DATA_LOGGER_H__


#include "DataLogger.h"
#include <string>
#include "HumanoidStateMachineController.h"
using namespace std;

class HumanoidDataLogger : public DataLogger, public HumanoidStateMachineController 
{
	
public:	
	HumanoidDataLogger(dmArticulation * robot, int stateSize);
	void logData();
	void saveData();
	string dataSaveDirectory;
private:
	int LEFT_FOOT_POS, LEFT_FOOT_POS_DES, LEFT_FOOT_VEL, LEFT_FOOT_VEL_DES, LEFT_FOOT_ACC_DES;
	int RIGHT_FOOT_POS, RIGHT_FOOT_POS_DES, RIGHT_FOOT_VEL, RIGHT_FOOT_VEL_DES, RIGHT_FOOT_ACC_DES;
	int COM_POSITION, COM_VELOCITY, COM_POSITION_DES, COM_VELOCITY_DES, CENTROIDAL_MOMENTUM, QDD_OPT, HMAT, HDOT_DES, HDOT_OPT;
	
	enum DataItems {
		TIME,
		STATE_CODE,
		// POSITIONS
		BASE_QUAT0,
		BASE_QUAT1,
		BASE_QUAT2,
		BASE_QUAT3,
		BASE_P_X,
		BASE_P_Y,
		BASE_P_Z,
		RHIP_PHI,
		RHIP_PSI,
		RHIP_GAMMA,
		RKNEE,
		RANK1,
		RANK2,
		LHIP_PHI,
		LHIP_PSI,
		LHIP_GAMMA,
		LKNEE,
		LANK1,
		LANK2,
		RSHOULD_PHI,
		RSHOULD_PSI,
		RSHOULD_GAMMA,
		RELBOW,
		LSHOULD_PHI,
		LSHOULD_PSI,
		LSHOULD_GAMMA,
		LELBOW,
		// RATES
		BASE_OMEGA_X,
		BASE_OMEGA_Y,
		BASE_OMEGA_Z,
		BASE_V_X,
		BASE_V_Y,
		BASE_V_Z,
		RHIP_OMEGA_X,
		RHIP_OMEGA_Y,
		RHIP_OMEGA_Z,
		RKNEE_RATE,
		RANK1_RATE,
		RANK2_RATE,
		LHIP_OMEGA_X,
		LHIP_OMEGA_Y,
		LHIP_OMEGA_Z,
		LKNEE_RATE,
		LANK1_RATE,
		LANK2_RATE,
		RSHOULD_OMEGA_X,
		RSHOULD_OMEGA_Y,
		RSHOULD_OMEGA_Z,
		RELBOW_RATE,
		LSHOULD_OMEGA_X,
		LSHOULD_OMEGA_Y,
		LSHOULD_OMEGA_Z,
		LELBOW_RATE,
		// TORQUES
		RHIP_TAU_X,
		RHIP_TAU_Y,
		RHIP_TAU_Z,
		RKNEE_TAU,
		RANK1_TAU,
		RANK2_TAU,
		LHIP_TAU_X,
		LHIP_TAU_Y,
		LHIP_TAU_Z,
		LKNEE_TAU,
		LANK1_TAU,
		LANK2_TAU,
		RSHOULD_TAU_X,
		RSHOULD_TAU_Y,
		RSHOULD_TAU_Z,
		RELBOW_TAU,
		LSHOULD_TAU_X,
		LSHOULD_TAU_Y,
		LSHOULD_TAU_Z,
		LELBOW_TAU,
		// Centroial quantities
		/*	COM_P_X,
		 COM_P_Y,
		 COM_P_Z,
		 COM_V_X,
		 COM_V_Y,
		 COM_V_Z,
		 CM_K_X,
		 CM_K_Y,
		 CM_K_Z,
		 CM_L_X,
		 CM_L_Y,
		 CM_L_Z,*/
		// GRF Quantities
		LCOP_F_X,
		LCOP_F_Y,
		LCOP_F_Z,
		LCOP_P_X,
		LCOP_P_Y,
		LCOP_N_Z,
		RCOP_F_X,
		RCOP_F_Y,
		RCOP_F_Z,
		RCOP_P_X,
		RCOP_P_Y,
		RCOP_N_Z,
		ZMP_F_X,
		ZMP_F_Y,
		ZMP_F_Z,
		ZMP_P_X,
		ZMP_P_Y,
		ZMP_N_Z,
		MAX_STATIC_ITEMS
	};
	
	enum DataGroups {
		JOINT_ANGLES,
		JOINT_RATES,
		JOINT_TORQUES,
		LEFT_FOOT_WRENCH,
		//LEFT_FOOT_COP_FORCE,
		//LEFT_FOOT_COP_LOC,
		RIGHT_FOOT_WRENCH,
		//RIGHT_FOOT_COP_FORCE,
		//RIGHT_FOOT_COP_POS,
		ZMP_WRENCH,
		//ZMP_FORCE,
		//ZMP_POS,
		MAX_STATIC_GROUPS
	};
};

#endif