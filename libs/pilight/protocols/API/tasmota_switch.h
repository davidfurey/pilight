/*
  Copyright (C) CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PROTOCOL_TASMOTA_SWITCH_H_
#define _PROTOCOL_TASMOTA_SWITCH_H_

#include "../protocol.h"

extern struct protocol_t *tasmotaSwitch;
void tasmotaSwitchInit(void);

#endif
