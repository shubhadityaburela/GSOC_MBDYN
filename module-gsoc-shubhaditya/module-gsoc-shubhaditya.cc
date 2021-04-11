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

#include "mbconfig.h"           /* This goes first in every *.c,*.cc file */

#include <cmath>
#include <cfloat>

#include "dataman.h"
#include "drive.h"

class CustomDriveCaller : public DriveCaller {
private:
	DriveCaller *m_CDC;
	doublereal m_lower_limit;
	doublereal m_upper_limit;
	unsigned int m_num_intervals;

public:
	CustomDriveCaller(DriveCaller *CDC, doublereal lower_limit, 
		doublereal upper_limit, unsigned int num_intervals);
	virtual ~CustomDriveCaller(void);

	/* Copia */
	virtual DriveCaller* pCopy(void) const;

	/* Scrive il contributo del DriveCaller al file di restart */
	virtual std::ostream& Restart(std::ostream& out) const;

	inline doublereal dGet(const doublereal&  dVar) const;

	/* this is about drives that are differentiable */
	virtual bool bIsDifferentiable(void) const;
	virtual doublereal dGetP(const doublereal& dVar) const;

};

CustomDriveCaller::CustomDriveCaller(DriveCaller *CDC,
	doublereal lower_limit, doublereal upper_limit, 
	unsigned int num_intervals)
	: DriveCaller(0),
	m_CDC(CDC),
	m_lower_limit(lower_limit),
	m_upper_limit(upper_limit),
	m_num_intervals(num_intervals)
{
	NO_OP;
}

CustomDriveCaller::~CustomDriveCaller(void)
{
	NO_OP;
}

DriveCaller *
CustomDriveCaller::pCopy(void) const
{
	return new CustomDriveCaller(m_CDC->pCopy(),
	m_lower_limit, m_upper_limit, m_num_intervals);
}

std::ostream&
CustomDriveCaller::Restart(std::ostream& out) const
{
	return out << "Custom_drive, ", m_CDC->Restart(out)
				<< ", " << m_lower_limit
				<< ", " << m_upper_limit
				<< ", " << m_num_intervals;
}

inline doublereal
CustomDriveCaller::dGet(const doublereal& dVar) const
{
	doublereal h = (m_upper_limit - m_lower_limit)/m_num_intervals;
	doublereal sum = 0.;
	for(unsigned int k = 1; k < m_num_intervals; ++k){
		doublereal sub_int = m_lower_limit + k * h;
		sum += m_CDC->dGet(sub_int);
	}
	doublereal  integral = h * ((m_CDC->dGet(m_lower_limit) + 
		m_CDC->dGet(m_upper_limit))/2. + sum);
	
			
	return integral;
}


inline bool
CustomDriveCaller::bIsDifferentiable(void) const
{
	return m_CDC->bIsDifferentiable();
}


inline doublereal
CustomDriveCaller::dGetP(const doublereal& dVar) const
{
	
	ASSERT(bIsDifferentiable());
	
	doublereal h = (m_upper_limit - m_lower_limit)/m_num_intervals;
	doublereal sum = 0.;
	for(unsigned int k = 1; k < m_num_intervals; ++k){
		doublereal sub_int = m_lower_limit + k * h;
		sum += m_CDC->dGetP(sub_int);
	}
	doublereal  integral = h * ((m_CDC->dGetP(m_lower_limit) + 
		m_CDC->dGetP(m_upper_limit))/2. + sum);


	return integral;
}


/* prototype of the functional object: reads a drive caller */
struct CDR : public DriveCallerRead {
	virtual DriveCaller *
		Read(const DataManager* pDM, MBDynParser& HP, bool bDeferred) {
		DriveCaller *CDC = HP.GetDriveCaller(); 
		doublereal lower_limit(HP.GetReal());
		doublereal upper_limit(HP.GetReal());
		unsigned int num_intervals(HP.GetInt());

		return new CustomDriveCaller(CDC, lower_limit, 
					upper_limit, num_intervals);
	};
};


extern "C" int
module_init(const char *module_name, void *pdm, void *php)
{
#if 0
	DataManager      *pDM = (DataManager *)pdm;
	MBDynParser      *pHP = (MBDynParser *)php;
#endif

	DriveCallerRead	*rf = new CDR;

	if (!SetDriveCallerData("Custom_drive", rf)) {
		delete rf;

		silent_cerr("CustomDrive: "
			"module_init(" << module_name << ") "
			"failed" << std::endl);

		return -1;
	}

	return 0;
}