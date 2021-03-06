
#define META_CMAES 0
#define CONDITION_CVT_ME 1
#define CONDITION_AURORA 2
#define CONDITION_CMAES_CHECK 3

//#define EXPERIMENT_TYPE CONDITION_DUTY_CYCLE

#define META() EXPERIMENT_TYPE == META_CMAES
#define CONTROL() EXPERIMENT_TYPE == CONDITION_CVT_ME
#define AURORA() EXPERIMENT_TYPE == CONDITION_AURORA
#define NOT_AURORA() EXPERIMENT_TYPE != CONDITION_AURORA

#define CMAES_CHECK() EXPERIMENT_TYPE == CONDITION_CMAES_CHECK
#define NOT_CMAES_CHECK() EXPERIMENT_TYPE != CONDITION_CMAES_CHECK
#define ENVIR_TESTS() EVAL_ENVIR == 1 && (TEST || META() || CMAES_CHECK())
#define DAMAGE_TESTS() EVAL_ENVIR == 0 && (TEST || META() || CMAES_CHECK())
#define USE_FEATURE_SETS() FEATURE_SETS && !BASE_BEHAVS


#ifdef GRAPHIC
#define NO_PARALLEL
#endif

#define SIMU_TIME 5

#define RASTRI_DIM 10 // random; just to avoid compilation error



#define FRICTION 1.0

//#define TAKE_COMPLEMENT // whether or not to test for generalisation to unseen world/damage conditions

//#define PRINTING
//#define CHECK_PARALLEL
#ifndef TEST
#if NUM_CORES > 1
	#define PARALLEL_RUN
#endif
#endif
