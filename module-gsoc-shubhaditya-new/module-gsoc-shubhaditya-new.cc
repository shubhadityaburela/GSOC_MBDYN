/* $Header$ */
/*
 * MBDyn (C) is a multibody analysis code.
 * http://www.mbdyn.org
 *
 * Copyright (C) 1996-2017
 *
 * Pierangelo Masarati	<masarati@aero.polimi.it>
 * Paolo Mantegazza	<mantegazza@aero.polimi.it>
 *
 * Dipartimento di Ingegneria Aerospaziale - Politecnico di Milano
 * via La Masa, 34 - 20156 Milano, Italy
 * http://www.aero.polimi.it
 *
 * Changing this copyright notice is forbidden.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 2 of the License).
 *
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "mbconfig.h"           /* This goes first in every *.c,*.cc file */
#endif

#include <cmath>
#include <cfloat>
#include <myassert.h>
#include <except.h>
#include <math.h>

#include <dataman.h>
#include <drive.h>

#include "module-gsoc-shubhaditya-new.h"


class LogarithicDrive : public DriveCaller {
	
private:
	const doublereal m_amplitude_value;
	const doublereal m_time_constant_value;
	const doublereal m_initial_time;
	const doublereal m_initial_value;

public:
	explicit LogarithicDrive(const DriveHandler* pDH, const doublereal amplitude_value, 
		const doublereal time_constant_value, const doublereal initial_time,
		const doublereal initial_value);
	virtual ~LogarithicDrive(void);

	virtual DriveCaller* pCopy(void) const;
	virtual std::ostream& Restart(std::ostream& out) const;
	inline doublereal dGet(const doublereal&  dVar) const;

	virtual bool bIsDifferentiable(void) const;
	virtual doublereal dGetP(const doublereal& dVar) const;

};


/* prototype of the functional object: reads a drive caller */
struct LogarithicDriveDCR : public DriveCallerRead {
	DriveCaller *
		Read(const DataManager* pDM, MBDynParser& HP, bool bDeferred) {
			
			NeedDM(pDM, HP, bDeferred, "logarithmic");
			
			const DriveHandler* pDrvHdl = 0;
			
			if(pDM != 0){
				pDrvHdl = pDM->pGetDrvHdl();
			}
			
			if(pDM == 0){
				silent_cerr("sorry, since the driver is not owned by a DataManager" << std::endl
					<< "no DOF dependent drivers are allowed;" << std::endl
					<< "aborting..." << std::endl);
				throw DataManager::ErrGeneric(MBDYN_EXCEPT_ARGS);
			}
			
			DriveCaller *pDC = 0;
			
			const doublereal amplitude_value = HP.GetReal();
			const doublereal time_constant_value = HP.GetReal();
			const doublereal initial_time = HP.GetReal();
			const doublereal initial_value = HP.GetReal();
			
			
			SAFENEWWITHCONSTRUCTOR(pDC,
				LogarithicDrive,
				LogarithicDrive(pDrvHdl, amplitude_value, 
					time_constant_value, initial_time, 
					initial_value));
			


			return pDC;
	};
};


LogarithicDrive::LogarithicDrive(const DriveHandler* pDH,
	const doublereal amplitude_value, const doublereal time_constant_value, 
	const doublereal initial_time, const doublereal initial_value)
	: DriveCaller(pDH),
	m_amplitude_value(amplitude_value),
	m_time_constant_value(time_constant_value),
	m_initial_time(initial_time),
	m_initial_value(initial_value)
{
	NO_OP;
}

LogarithicDrive::~LogarithicDrive(void)
{
	NO_OP;
}

DriveCaller *
LogarithicDrive::pCopy(void) const
{
	DriveCaller* pDC = 0;
	
	SAFENEWWITHCONSTRUCTOR(pDC,
			LogarithicDrive,
			LogarithicDrive(pGetDrvHdl(), m_amplitude_value,
				m_time_constant_value, m_initial_time,
				m_initial_value));

	return pDC;
}

// Restart
std::ostream&
LogarithicDrive::Restart(std::ostream& out) const
{
	return out << "logarithmic, "
				<< ", " << m_amplitude_value
				<< ", " << m_time_constant_value
				<< ", " << m_initial_time
				<< ", " << m_initial_value;
}

inline doublereal
LogarithicDrive::dGet(const doublereal& dVar) const
{
	
	doublereal log_arg = (dVar - m_initial_time)/m_time_constant_value;
	if(log_arg <= 1.){
		return 1.;
	}
	else{
		return (m_initial_value + m_amplitude_value * log(log_arg));
	}
	
}

inline bool
LogarithicDrive::bIsDifferentiable(void) const
{
	return false;
}

inline doublereal
LogarithicDrive::dGetP(const doublereal& dVar) const
{
	return 0.;
}

bool
logarithmic_drive_set()
{
	DriveCallerRead *rf = new LogarithicDriveDCR;
	
	if (!SetDriveCallerData("logarithmic", rf)) {
		delete rf;
		return false;
	}
	
	return true;
}



#ifndef STATIC_MODULES

extern "C" int
module_init(const char *module_name, void *pdm, void *php)
{
#if 0
	DataManager      *pDM = (DataManager *)pdm;
	MBDynParser      *pHP = (MBDynParser *)php;
#endif

	if(!logarithmic_drive_set()){
		silent_cerr("logarithmic_drive: "
			"module_init(" << module_name << ") "
			"failed" << std::endl);

		return -1;
	}

	return 0;
}

#endif