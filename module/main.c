/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "kinterface.h"

struct kinterface* kinterface;

kuint _start(struct kinterface* _kinterface) {
	kuint8** info;

	kinterface = _kinterface;

	if(!kinterface->kinterface_check_version(INTERFACE_VERSION, KERNEL_VERSION)) {
		return 1;
	}

	info = kinterface->kinterface_kernel_info();
	if(info) {
		kuint i;

		for(i = 0; info[i]; i++) {
			kinterface->kdebug("%s\n", info[i]);
		}
	}

	return 0;
}

