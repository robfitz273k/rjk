/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#ifndef _kimplementation_h
#define _kimplementation_h

/*
 * Kernel Interface: Data Type's
 */

typedef signed long int kint;
typedef signed char kint8;
typedef signed short int kint16;
typedef signed long int kint32;
typedef signed long long int kint64;

typedef unsigned long int kuint;
typedef unsigned char kuint8;
typedef unsigned short int kuint16;
typedef unsigned long int kuint32;
typedef unsigned long long int kuint64;

typedef long double kfloat;
typedef float kfloat32;
typedef double kfloat64;

typedef kuint kirq;
typedef kuint kmonitor;
typedef kuint kthread;
typedef kuint ktimer;
typedef kuint kvirtual;

#define ktrue (1)
#define kfalse (0)
#define knull ((void*)0)
#define kpage_size (4096)

#define kfunction inline

/*
 * Kernel Interface: Variable Argument's
 */

#include <stdarg.h>

/*
 * Kernel Interface: Kernel Initialize
 */

/*
 * Kernel Interface: Atomic Operation's
 */

/*
 * Kernel Interface: Debugging
 */

void kdebug_init();

/*
 * Kernel Interface: IRQ's
 */

void kirq_init();

/*
 * Kernel Interface: Input Output Port's
 */

/*
 * Kernel Interface: Inter Thread Communication
 */

/*
 * Kernel Interface: Monitor's
 */

/*
 * Kernel Interface: Thread's
 */

void kthread_init();

/*
 * Kernel Interface: Time
 */

void ktime_init();

/*
 * Kernel Interface: Timer's
 */

/*
 * Kernel Interface: Memory
 */

void kmemory_init();

#endif /* _kimplementation_h */

