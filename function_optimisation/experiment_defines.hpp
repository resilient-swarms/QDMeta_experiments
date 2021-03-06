
#define META_CMAES 0
#define CONDITION_CVT_ME 1
#define CONDITION_AURORA 2
#define CONDITION_CMAES_CHECK 3

#define META() EXPERIMENT_TYPE == META_CMAES
#define CONTROL() EXPERIMENT_TYPE == CONDITION_CVT_ME
#define AURORA() EXPERIMENT_TYPE == CONDITION_AURORA
#define NOT_AURORA() EXPERIMENT_TYPE != CONDITION_AURORA

#define CMAES_CHECK() EXPERIMENT_TYPE == CONDITION_CMAES_CHECK
#define NOT_CMAES_CHECK() EXPERIMENT_TYPE != CONDITION_CMAES_CHECK
#define DIMENSION_TESTS() PERTURB_TYPE == 1 && (TEST || META() || CMAES_CHECK())
#define TRANSLATION_TESTS() PERTURB_TYPE == 2 && (TEST || META() || CMAES_CHECK())

#ifdef GRAPHIC
#define NO_PARALLEL
#endif

#define MAXFIT 2000.0f   // use for out-of-bound solutions in meta-fitness and scaling param control
#define RASTRI_MIN -5.12f
#define RASTRI_MAX 5.12f
#define RASTRI_RANGE 2*RASTRI_MAX

#define SIMU_TIME 5 // arbitrary; just to avoid compilation errors

#ifndef TEST
#if NUM_CORES > 1
	#define PARALLEL_RUN
#endif
#endif