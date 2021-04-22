#ifndef RTW_HEADER_AUV_acc_h_
#define RTW_HEADER_AUV_acc_h_
#include <stddef.h>
#include <float.h>
#include <string.h>
#ifndef AUV_acc_COMMON_INCLUDES_
#define AUV_acc_COMMON_INCLUDES_
#include <stdlib.h>
#define S_FUNCTION_NAME simulink_only_sfcn 
#define S_FUNCTION_LEVEL 2
#define RTW_GENERATED_S_FUNCTION
#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#endif
#include "AUV_acc_types.h"
#include "multiword_types.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"
#include "mwmathutil.h"
#include "rt_defines.h"
typedef struct { real_T B_5_0_0 [ 3 ] ; real_T B_5_2_0 ; real_T B_5_4_0 [ 3 ]
; real_T B_5_6_0 [ 3 ] ; real_T B_5_7_0 [ 3 ] ; real_T B_5_8_0 [ 3 ] ; real_T
B_5_9_0 [ 3 ] ; real_T B_5_10_0 [ 3 ] ; real_T B_5_11_0 [ 3 ] ; real_T
B_5_12_0 [ 3 ] ; real_T B_5_13_0 [ 3 ] ; real_T B_5_14_0 [ 3 ] ; real_T
B_5_15_0 ; real_T B_5_16_0 ; real_T B_5_17_0 ; real_T B_5_18_0 [ 4 ] ; real_T
B_5_19_0 [ 3 ] ; real_T B_5_21_0 [ 9 ] ; real_T B_5_22_0 [ 9 ] ; real_T
B_5_23_0 [ 12 ] ; real_T B_5_26_0 ; real_T B_5_0_0_m ; real_T B_5_1_0 ;
real_T B_4_0_1 [ 3 ] ; real_T B_3_0_1 [ 4 ] ; real_T B_2_0_1 ; real_T B_1_0_0
[ 3 ] ; real_T B_1_2_0 [ 3 ] ; real_T B_0_0_1 [ 3 ] ; real_T B_0_0_2 [ 9 ] ;
real_T B_0_0_3 [ 9 ] ; real_T B_0_0_4 [ 12 ] ; real_T B_0_0_5 [ 3 ] ; }
B_AUV_T ; typedef struct { real_T TimeStampA ; real_T LastUAtTimeA [ 3 ] ;
real_T TimeStampB ; real_T LastUAtTimeB [ 3 ] ; real_T TimeStampA_m ; real_T
LastUAtTimeA_e [ 3 ] ; real_T TimeStampB_o ; real_T LastUAtTimeB_e [ 3 ] ;
real_T TimeStampA_l ; real_T LastUAtTimeA_b [ 3 ] ; real_T TimeStampB_n ;
real_T LastUAtTimeB_j [ 3 ] ; real_T IC_FirstOutputTime ; real_T
IC_FirstOutputTime_k ; real_T IC_FirstOutputTime_p ; real_T
IC1_FirstOutputTime ; real_T IC2_FirstOutputTime ; real_T NextOutput ; real_T
TimeStampA_k ; real_T LastUAtTimeA_b5 [ 3 ] ; real_T TimeStampB_p ; real_T
LastUAtTimeB_g [ 3 ] ; real_T IC1_FirstOutputTime_h ; struct { real_T
modelTStart ; } TransportDelay_RWORK ; void * ToWorkspace_PWORK ; void *
ToWorkspace1_PWORK ; struct { void * TUbufferPtrs [ 8 ] ; }
TransportDelay_PWORK ; void * ToWorkspace2_PWORK ; void *
Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Task_0_AlgLoopData ; int32_T
ReferenceTrajectory_sysIdxToRun ; int32_T ControlLaw_sysIdxToRun ; int32_T
AverageCurrentModel_sysIdxToRun ; int32_T
Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Task_0_sysIdxToRun ; int32_T
Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Task_0_blkIdxToRun ; int32_T
Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Task_0_parentSysIdxToRun ;
int32_T planarDynamics_sysIdxToRun ; uint32_T RandSeed ; struct { int_T Tail
[ 4 ] ; int_T Head [ 4 ] ; int_T Last [ 4 ] ; int_T CircularBufSize [ 4 ] ;
int_T MaxNewBufSize ; } TransportDelay_IWORK ; char_T
pad_TransportDelay_IWORK [ 4 ] ; } DW_AUV_T ; typedef struct { real_T
Integrator1_CSTATE [ 3 ] ; real_T Integrator_CSTATE [ 3 ] ; real_T
Integrator_CSTATE_m [ 3 ] ; real_T Integrator_CSTATE_p ; } X_AUV_T ; typedef
struct { real_T Integrator1_CSTATE [ 3 ] ; real_T Integrator_CSTATE [ 3 ] ;
real_T Integrator_CSTATE_m [ 3 ] ; real_T Integrator_CSTATE_p ; } XDot_AUV_T
; typedef struct { boolean_T Integrator1_CSTATE [ 3 ] ; boolean_T
Integrator_CSTATE [ 3 ] ; boolean_T Integrator_CSTATE_m [ 3 ] ; boolean_T
Integrator_CSTATE_p ; } XDis_AUV_T ; typedef struct { real_T
Integrator1_CSTATE [ 3 ] ; real_T Integrator_CSTATE [ 3 ] ; real_T
Integrator_CSTATE_m [ 3 ] ; real_T Integrator_CSTATE_p ; } CStateAbsTol_AUV_T
; typedef struct { real_T Integrator1_CSTATE [ 3 ] ; real_T Integrator_CSTATE
[ 3 ] ; real_T Integrator_CSTATE_m [ 3 ] ; real_T Integrator_CSTATE_p ; }
CXPtMin_AUV_T ; typedef struct { real_T Integrator1_CSTATE [ 3 ] ; real_T
Integrator_CSTATE [ 3 ] ; real_T Integrator_CSTATE_m [ 3 ] ; real_T
Integrator_CSTATE_p ; } CXPtMax_AUV_T ; struct P_AUV_T_ { real_T P_0 [ 3 ] ;
real_T P_1 ; real_T P_2 ; real_T P_3 ; real_T P_4 ; real_T P_5 ; real_T P_6 ;
real_T P_7 ; real_T P_8 ; real_T P_9 ; real_T P_10 ; real_T P_11 [ 4 ] ;
real_T P_12 [ 3 ] ; real_T P_13 [ 9 ] ; real_T P_14 [ 9 ] ; real_T P_15 [ 12
] ; real_T P_16 ; real_T P_17 ; real_T P_18 ; real_T P_19 ; real_T P_20 ; } ;
extern P_AUV_T AUV_rtDefaultP ;
#endif
