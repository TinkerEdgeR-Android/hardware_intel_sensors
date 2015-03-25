/*
 * Copyright (C) 2013-2015 STMicroelectronics
 * Author: Denis Ciocca - <denis.ciocca@st.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ST_SW_ACCEL_MAGN_6X_FUSION_H
#define ST_SW_ACCEL_MAGN_6X_FUSION_H

#include "SWSensorBase.h"

#define ST_GEOMAG_ROTATION_VECTOR_OUT_ID		(0)
#define ST_GEOMAG_ORIENTATION_OUT_ID			(1)
#define ST_GEOMAG_LINEAR_ACCEL_OUT_ID			(2)
#define ST_GEOMAG_GRAVITY_OUT_ID			(3)
#define ST_GEOMAG_MAX_OUT_ID				(4)

class SWAccelMagnFusion6X : public SWSensorBaseWithPollrate {
protected:
	SensorBaseData outdata[ST_GEOMAG_MAX_OUT_ID];

public:
	SWAccelMagnFusion6X(const char *name, int handle, int pipe_data_fd);
	~SWAccelMagnFusion6X();

	virtual int Enable(int handle, bool enable);
	virtual int SetDelay(int handle, int64_t period_ns, int64_t timeout);
	virtual void SplitAndProcessData(SensorBaseData data[ST_GEOMAG_MAX_OUT_ID]);
	virtual void TriggerEventReceived();
};

#endif /* ST_SW_ACCEL_MAGN_6X_FUSION_H */