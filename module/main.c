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

	if(!kinterface->kinterface_check_version(KERNEL_INTERFACE_VERSION, KERNEL_IMPLEMENTATION_VERSION)) {
		return 1;
	}

	info = kinterface->kinterface_kernel_info();
	if(info) {
		kuint i;

		for(i = 0; info[i]; i++) {
			kinterface->kprintf("%s\n", info[i]);
		}
	}

	{
		kfloat fu = 0;
		kfloat fv = 0;
		kfloat fr = 0;

		kinterface->kprintf("%f %f %f\n", fu, fv, fr);

		fu = 12434384.4243;

		kinterface->kprintf("%f %f %f\n", fu, fv, fr);

		fv = 1244.42;

		kinterface->kprintf("%f %f %f\n", fu, fv, fr);

		fr = fu / fv;

		kinterface->kprintf("%f %f %f\n", fu, fv, fr);
	}

	return 0;
}

