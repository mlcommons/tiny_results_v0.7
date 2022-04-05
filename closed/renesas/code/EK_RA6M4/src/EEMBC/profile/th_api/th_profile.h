/**
 * Copyright (C) EEMBC(R). All Rights Reserved
 *
 * All EEMBC Benchmark Software are products of EEMBC and are provided under the
 * terms of the EEMBC Benchmark License Agreements. The EEMBC Benchmark Software
 * are proprietary intellectual properties of EEMBC and its Members and is
 * protected under all applicable laws, including all applicable copyright laws.
 *
 * If you received this EEMBC Benchmark Software without having a currently
 * effective EEMBC Benchmark License Agreement, you must discontinue use.
 */


#ifndef __TH_PROFILE_H
#define __TH_PROFILE_H

#include "profile/ee_profile.h"
#include "profile/th_api/th_util.h"

/* PORTME:
 * Select an EE_MODEL_VERSION_* from ee_profile.h, so that the Host UI
 * knows which model is inside this DUT!
 * */
#ifdef EEMBCKWS
	#define TH_MODEL_VERSION EE_MODEL_VERSION_AWWS01    // KWS      (Keyword Spotting)
#endif
#ifdef EEMBCVWW
	#define TH_MODEL_VERSION EE_MODEL_VERSION_VWW01     //          (Visual wake word, aka person detection)
#endif
#ifdef EEMBCAD
	#define TH_MODEL_VERSION EE_MODEL_VERSION_AD01      //          (Anomaly Detection)
#endif
#ifdef EEMBCIC
	#define TH_MODEL_VERSION EE_MODEL_VERSION_IC01      // Cifar10  (Image Classification)
#endif

#endif
