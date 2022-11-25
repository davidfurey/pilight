/*
	Copyright (C) CurlyMo and MaGallant

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PROTOCOL_BME280_H_
#define _PROTOCOL_BME280_H_

#include "../protocol.h"

extern struct protocol_t *bme280;
void bme280Init(void);

#endif
