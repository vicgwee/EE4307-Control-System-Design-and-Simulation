#include <math.h>
#include "AUV_acc.h"
#include "AUV_acc_private.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simtarget/slSimTgtMdlrefSfcnBridge.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T AUV_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T *
tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * *
tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf ,
boolean_T istransportdelay , int_T * maxNewBufSzPtr ) { int_T testIdx ; int_T
tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf = * tBufPtr ;
real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if ( istransportdelay ) {
numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail < ( bufSz - 1 ) ) ? (
tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] ) && ! isfixedbuf ) {
int_T j ; real_T * tempT ; real_T * tempU ; real_T * tempX = ( NULL ) ;
real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024 ; if ( newBufSz > *
maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU = ( real_T * )
utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if ( tempU == ( NULL
) ) { return ( false ) ; } tempT = tempU + newBufSz ; if ( istransportdelay )
tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j ++ ) { tempT [ j -
tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if ( istransportdelay
) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j < tail ; j ++ ) { tempT
[ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz - tail ] = uBuf [ j ] ;
if ( istransportdelay ) tempX [ j + bufSz - tail ] = xBuf [ j ] ; } if ( *
lastPtr > tail ) { * lastPtr -= tail ; } else { * lastPtr += ( bufSz - tail )
; } * tailPtr = 0 ; * headPtr = bufSz ; utFree ( uBuf ) ; * bufSzPtr =
newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if ( istransportdelay ) *
xBufPtr = tempX ; } else { * tailPtr = testIdx ; } return ( true ) ; } real_T
AUV_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart , real_T *
tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T oldestIdx ,
int_T newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) { int_T i ; real_T yout , t1 , t2 , u1 , u2
; if ( ( newIdx == 0 ) && ( oldestIdx == 0 ) && ( tMinusDelay > tStart ) )
return initOutput ; if ( tMinusDelay <= tStart ) return initOutput ; if ( (
tMinusDelay <= tBuf [ oldestIdx ] ) ) { if ( discrete ) { return ( uBuf [
oldestIdx ] ) ; } else { int_T tempIdx = oldestIdx + 1 ; if ( oldestIdx ==
bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [ oldestIdx ] ; t2 = tBuf [ tempIdx ] ;
u1 = uBuf [ oldestIdx ] ; u2 = uBuf [ tempIdx ] ; if ( t2 == t1 ) { if (
tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else { real_T f1 =
( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout = f1 * u1 +
f2 * u2 ; } return yout ; } } if ( minorStepAndTAtLastMajorOutput ) { if (
newIdx != 0 ) { if ( * lastIdx == newIdx ) { ( * lastIdx ) -- ; } newIdx -- ;
} else { if ( * lastIdx == newIdx ) { * lastIdx = bufSz - 1 ; } newIdx =
bufSz - 1 ; } } i = * lastIdx ; if ( tBuf [ i ] < tMinusDelay ) { while (
tBuf [ i ] < tMinusDelay ) { if ( i == newIdx ) break ; i = ( i < ( bufSz - 1
) ) ? ( i + 1 ) : 0 ; } } else { while ( tBuf [ i ] >= tMinusDelay ) { i = (
i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0
; } * lastIdx = i ; if ( discrete ) { double tempEps = ( DBL_EPSILON ) *
128.0 ; double localEps = tempEps * muDoubleScalarAbs ( tBuf [ i ] ) ; if (
tempEps > localEps ) { localEps = tempEps ; } localEps = localEps / 2.0 ; if
( tMinusDelay >= ( tBuf [ i ] - localEps ) ) { yout = uBuf [ i ] ; } else {
if ( i == 0 ) { yout = uBuf [ bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ;
} } } else { if ( i == 0 ) { t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1
] ; } else { t1 = tBuf [ i - 1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ;
u2 = uBuf [ i ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; }
else { yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 )
; real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } } return ( yout ) ; }
void rt_ssGetBlockPath ( SimStruct * S , int_T sysIdx , int_T blkIdx , char_T
* * path ) { _ssGetBlockPath ( S , sysIdx , blkIdx , path ) ; } void
rt_ssSet_slErrMsg ( SimStruct * S , void * diag ) { if ( !
_ssIsErrorStatusAslErrMsg ( S ) ) { _ssSet_slErrMsg ( S , diag ) ; } else {
_ssDiscardDiagnostic ( S , diag ) ; } } void rt_ssReportDiagnosticAsWarning (
SimStruct * S , void * diag ) { _ssReportDiagnosticAsWarning ( S , diag ) ; }
real_T rt_urand_Upu32_Yd_f_pw_snf ( uint32_T * u ) { uint32_T hi ; uint32_T
lo ; lo = * u % 127773U * 16807U ; hi = * u / 127773U * 2836U ; if ( lo < hi
) { * u = 2147483647U - ( hi - lo ) ; } else { * u = lo - hi ; } return (
real_T ) * u * 4.6566128752457969E-10 ; } real_T rt_nrand_Upu32_Yd_f_pw_snf (
uint32_T * u ) { real_T si ; real_T sr ; real_T y ; do { sr = 2.0 *
rt_urand_Upu32_Yd_f_pw_snf ( u ) - 1.0 ; si = 2.0 *
rt_urand_Upu32_Yd_f_pw_snf ( u ) - 1.0 ; si = sr * sr + si * si ; } while (
si > 1.0 ) ; y = muDoubleScalarSqrt ( - 2.0 * muDoubleScalarLog ( si ) / si )
* sr ; return y ; } static void mdlOutputs ( SimStruct * S , int_T tid ) {
B_AUV_T * _rtB ; DW_AUV_T * _rtDW ; P_AUV_T * _rtP ; X_AUV_T * _rtX ; real_T
( * lastU ) [ 3 ] ; real_T lastTime ; int32_T isHit ; _rtDW = ( ( DW_AUV_T *
) ssGetRootDWork ( S ) ) ; _rtX = ( ( X_AUV_T * ) ssGetContStates ( S ) ) ;
_rtP = ( ( P_AUV_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_AUV_T * )
_ssGetModelBlockIO ( S ) ) ; _rtB -> B_5_0_0 [ 0 ] = _rtX ->
Integrator1_CSTATE [ 0 ] ; _rtB -> B_5_0_0 [ 1 ] = _rtX -> Integrator1_CSTATE
[ 1 ] ; _rtB -> B_5_0_0 [ 2 ] = _rtX -> Integrator1_CSTATE [ 2 ] ; if ( (
_rtDW -> IC_FirstOutputTime == ( rtMinusInf ) ) || ( _rtDW ->
IC_FirstOutputTime == ssGetTaskTime ( S , 0 ) ) ) { _rtDW ->
IC_FirstOutputTime = ssGetTaskTime ( S , 0 ) ; _rtB -> B_5_1_0 [ 0 ] = _rtP
-> P_2 [ 0 ] ; _rtB -> B_5_1_0 [ 1 ] = _rtP -> P_2 [ 1 ] ; _rtB -> B_5_1_0 [
2 ] = _rtP -> P_2 [ 2 ] ; } else { _rtB -> B_5_1_0 [ 0 ] = _rtB -> B_5_0_0 [
0 ] ; _rtB -> B_5_1_0 [ 1 ] = _rtB -> B_5_0_0 [ 1 ] ; _rtB -> B_5_1_0 [ 2 ] =
_rtB -> B_5_0_0 [ 2 ] ; } ssCallAccelRunBlock ( S , 5 , 2 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_5_3_0 = ssGetT ( S ) ; ssCallAccelRunBlock
( S , 4 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_5_5_0 [ 0 ] = _rtB -> B_5_1_0
[ 0 ] - _rtB -> B_4_0_1 [ 0 ] ; _rtB -> B_5_5_0 [ 1 ] = _rtB -> B_5_1_0 [ 1 ]
- _rtB -> B_4_0_1 [ 1 ] ; _rtB -> B_5_5_0 [ 2 ] = _rtB -> B_5_1_0 [ 2 ] -
_rtB -> B_4_0_1 [ 2 ] ; ssCallAccelRunBlock ( S , 5 , 6 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_5_7_0 [ 0 ] = _rtP -> P_3 * _rtB -> B_5_5_0 [ 0 ] ; _rtB ->
B_5_8_0 [ 0 ] = _rtX -> Integrator_CSTATE [ 0 ] ; _rtB -> B_5_9_0 [ 0 ] =
_rtP -> P_5 * _rtB -> B_5_8_0 [ 0 ] ; _rtB -> B_5_7_0 [ 1 ] = _rtP -> P_3 *
_rtB -> B_5_5_0 [ 1 ] ; _rtB -> B_5_8_0 [ 1 ] = _rtX -> Integrator_CSTATE [ 1
] ; _rtB -> B_5_9_0 [ 1 ] = _rtP -> P_5 * _rtB -> B_5_8_0 [ 1 ] ; _rtB ->
B_5_7_0 [ 2 ] = _rtP -> P_3 * _rtB -> B_5_5_0 [ 2 ] ; _rtB -> B_5_8_0 [ 2 ] =
_rtX -> Integrator_CSTATE [ 2 ] ; _rtB -> B_5_9_0 [ 2 ] = _rtP -> P_5 * _rtB
-> B_5_8_0 [ 2 ] ; if ( ( _rtDW -> TimeStampA >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB >= ssGetT ( S ) ) ) { _rtB -> B_5_10_0 [ 0 ] = 0.0 ; _rtB ->
B_5_10_0 [ 1 ] = 0.0 ; _rtB -> B_5_10_0 [ 2 ] = 0.0 ; } else { lastTime =
_rtDW -> TimeStampA ; lastU = & _rtDW -> LastUAtTimeA ; if ( _rtDW ->
TimeStampA < _rtDW -> TimeStampB ) { if ( _rtDW -> TimeStampB < ssGetT ( S )
) { lastTime = _rtDW -> TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ; } }
else { if ( _rtDW -> TimeStampA >= ssGetT ( S ) ) { lastTime = _rtDW ->
TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ; } } lastTime = ssGetT ( S ) -
lastTime ; _rtB -> B_5_10_0 [ 0 ] = ( _rtB -> B_5_5_0 [ 0 ] - ( * lastU ) [ 0
] ) / lastTime ; _rtB -> B_5_10_0 [ 1 ] = ( _rtB -> B_5_5_0 [ 1 ] - ( * lastU
) [ 1 ] ) / lastTime ; _rtB -> B_5_10_0 [ 2 ] = ( _rtB -> B_5_5_0 [ 2 ] - ( *
lastU ) [ 2 ] ) / lastTime ; } _rtB -> B_5_11_0 [ 0 ] = _rtP -> P_6 * _rtB ->
B_5_10_0 [ 0 ] ; _rtB -> B_5_12_0 [ 0 ] = ( _rtB -> B_5_7_0 [ 0 ] + _rtB ->
B_5_9_0 [ 0 ] ) + _rtB -> B_5_11_0 [ 0 ] ; _rtB -> B_5_13_0 [ 0 ] = _rtX ->
Integrator_CSTATE_m [ 0 ] ; _rtB -> B_5_11_0 [ 1 ] = _rtP -> P_6 * _rtB ->
B_5_10_0 [ 1 ] ; _rtB -> B_5_12_0 [ 1 ] = ( _rtB -> B_5_7_0 [ 1 ] + _rtB ->
B_5_9_0 [ 1 ] ) + _rtB -> B_5_11_0 [ 1 ] ; _rtB -> B_5_13_0 [ 1 ] = _rtX ->
Integrator_CSTATE_m [ 1 ] ; _rtB -> B_5_11_0 [ 2 ] = _rtP -> P_6 * _rtB ->
B_5_10_0 [ 2 ] ; _rtB -> B_5_12_0 [ 2 ] = ( _rtB -> B_5_7_0 [ 2 ] + _rtB ->
B_5_9_0 [ 2 ] ) + _rtB -> B_5_11_0 [ 2 ] ; _rtB -> B_5_13_0 [ 2 ] = _rtX ->
Integrator_CSTATE_m [ 2 ] ; if ( ( _rtDW -> TimeStampA_m >= ssGetT ( S ) ) &&
( _rtDW -> TimeStampB_o >= ssGetT ( S ) ) ) { _rtB -> B_5_14_0 [ 0 ] = 0.0 ;
_rtB -> B_5_14_0 [ 1 ] = 0.0 ; _rtB -> B_5_14_0 [ 2 ] = 0.0 ; } else {
lastTime = _rtDW -> TimeStampA_m ; lastU = & _rtDW -> LastUAtTimeA_e ; if (
_rtDW -> TimeStampA_m < _rtDW -> TimeStampB_o ) { if ( _rtDW -> TimeStampB_o
< ssGetT ( S ) ) { lastTime = _rtDW -> TimeStampB_o ; lastU = & _rtDW ->
LastUAtTimeB_e ; } } else { if ( _rtDW -> TimeStampA_m >= ssGetT ( S ) ) {
lastTime = _rtDW -> TimeStampB_o ; lastU = & _rtDW -> LastUAtTimeB_e ; } }
lastTime = ssGetT ( S ) - lastTime ; _rtB -> B_5_14_0 [ 0 ] = ( _rtB ->
B_4_0_1 [ 0 ] - ( * lastU ) [ 0 ] ) / lastTime ; _rtB -> B_5_14_0 [ 1 ] = (
_rtB -> B_4_0_1 [ 1 ] - ( * lastU ) [ 1 ] ) / lastTime ; _rtB -> B_5_14_0 [ 2
] = ( _rtB -> B_4_0_1 [ 2 ] - ( * lastU ) [ 2 ] ) / lastTime ; } if ( ( _rtDW
-> TimeStampA_l >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB_n >= ssGetT ( S )
) ) { _rtB -> B_5_15_0 [ 0 ] = 0.0 ; _rtB -> B_5_15_0 [ 1 ] = 0.0 ; _rtB ->
B_5_15_0 [ 2 ] = 0.0 ; } else { lastTime = _rtDW -> TimeStampA_l ; lastU = &
_rtDW -> LastUAtTimeA_b ; if ( _rtDW -> TimeStampA_l < _rtDW -> TimeStampB_n
) { if ( _rtDW -> TimeStampB_n < ssGetT ( S ) ) { lastTime = _rtDW ->
TimeStampB_n ; lastU = & _rtDW -> LastUAtTimeB_j ; } } else { if ( _rtDW ->
TimeStampA_l >= ssGetT ( S ) ) { lastTime = _rtDW -> TimeStampB_n ; lastU = &
_rtDW -> LastUAtTimeB_j ; } } lastTime = ssGetT ( S ) - lastTime ; _rtB ->
B_5_15_0 [ 0 ] = ( _rtB -> B_5_14_0 [ 0 ] - ( * lastU ) [ 0 ] ) / lastTime ;
_rtB -> B_5_15_0 [ 1 ] = ( _rtB -> B_5_14_0 [ 1 ] - ( * lastU ) [ 1 ] ) /
lastTime ; _rtB -> B_5_15_0 [ 2 ] = ( _rtB -> B_5_14_0 [ 2 ] - ( * lastU ) [
2 ] ) / lastTime ; } _rtB -> B_5_16_0 = _rtX -> Integrator_CSTATE_p ; if ( (
_rtDW -> IC_FirstOutputTime_f == ( rtMinusInf ) ) || ( _rtDW ->
IC_FirstOutputTime_f == ssGetTaskTime ( S , 0 ) ) ) { _rtDW ->
IC_FirstOutputTime_f = ssGetTaskTime ( S , 0 ) ; _rtB -> B_5_17_0 = _rtP ->
P_9 ; } else { _rtB -> B_5_17_0 = _rtB -> B_5_16_0 ; } if ( _rtB -> B_5_1_0_c
> _rtP -> P_10 ) { _rtB -> B_5_18_0 = _rtB -> B_5_0_0_m ; } else { _rtB ->
B_5_18_0 = _rtB -> B_5_17_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 4 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay ; { int_T i1 ; real_T * y0 = & _rtB ->
B_5_19_0 [ 0 ] ; const real_T * p_P_12 = _rtP -> P_12 ; int_T * iw_Tail = &
_rtDW -> TransportDelay_IWORK . Tail [ 0 ] ; int_T * iw_Head = & _rtDW ->
TransportDelay_IWORK . Head [ 0 ] ; int_T * iw_Last = & _rtDW ->
TransportDelay_IWORK . Last [ 0 ] ; int_T * iw_CircularBufSize = & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 0 ] ; for ( i1 = 0 ; i1 < 4 ; i1 ++
) { tMinusDelay = ( ( _rtP -> P_11 > 0.0 ) ? _rtP -> P_11 : 0.0 ) ;
tMinusDelay = simTime - tMinusDelay ; y0 [ i1 ] =
AUV_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ] , iw_Tail [ i1 ] , iw_Head [ i1
] , p_P_12 [ i1 ] , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; tBuffer ++ ; uBuffer ++ ;
} } } ssCallAccelRunBlock ( S , 5 , _rtDW ->
Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Task_0_blkIdxToRun ,
SS_CALL_MDL_OUTPUTS ) ; if ( ( _rtDW -> IC_FirstOutputTime_p == ( rtMinusInf
) ) || ( _rtDW -> IC_FirstOutputTime_p == ssGetTaskTime ( S , 0 ) ) ) { _rtDW
-> IC_FirstOutputTime_p = ssGetTaskTime ( S , 0 ) ; memcpy ( & _rtB ->
B_5_21_0 [ 0 ] , & _rtP -> P_13 [ 0 ] , 9U * sizeof ( real_T ) ) ; } else {
memcpy ( & _rtB -> B_5_21_0 [ 0 ] , & _rtB -> B_0_0_2 [ 0 ] , 9U * sizeof (
real_T ) ) ; } if ( ( _rtDW -> IC1_FirstOutputTime == ( rtMinusInf ) ) || (
_rtDW -> IC1_FirstOutputTime == ssGetTaskTime ( S , 0 ) ) ) { _rtDW ->
IC1_FirstOutputTime = ssGetTaskTime ( S , 0 ) ; memcpy ( & _rtB -> B_5_22_0 [
0 ] , & _rtP -> P_14 [ 0 ] , 9U * sizeof ( real_T ) ) ; } else { memcpy ( &
_rtB -> B_5_22_0 [ 0 ] , & _rtB -> B_0_0_3 [ 0 ] , 9U * sizeof ( real_T ) ) ;
} if ( ( _rtDW -> IC2_FirstOutputTime == ( rtMinusInf ) ) || ( _rtDW ->
IC2_FirstOutputTime == ssGetTaskTime ( S , 0 ) ) ) { _rtDW ->
IC2_FirstOutputTime = ssGetTaskTime ( S , 0 ) ; memcpy ( & _rtB -> B_5_23_0 [
0 ] , & _rtP -> P_15 [ 0 ] , 12U * sizeof ( real_T ) ) ; } else { memcpy ( &
_rtB -> B_5_23_0 [ 0 ] , & _rtB -> B_0_0_4 [ 0 ] , 12U * sizeof ( real_T ) )
; } ssCallAccelRunBlock ( S , 3 , 0 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 5 , 25 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_5_26_0 = _rtDW ->
NextOutput ; } ssCallAccelRunBlock ( S , 2 , 0 , SS_CALL_MDL_OUTPUTS ) ;
UNUSED_PARAMETER ( tid ) ; } static void mdlOutputsTID2 ( SimStruct * S ,
int_T tid ) { B_AUV_T * _rtB ; P_AUV_T * _rtP ; _rtP = ( ( P_AUV_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_AUV_T * ) _ssGetModelBlockIO ( S ) ) ;
_rtB -> B_5_0_0_m = _rtP -> P_19 ; _rtB -> B_5_1_0_c = _rtP -> P_20 ;
UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { B_AUV_T * _rtB ;
DW_AUV_T * _rtDW ; P_AUV_T * _rtP ; real_T ( * lastU ) [ 3 ] ; int32_T isHit
; _rtDW = ( ( DW_AUV_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( ( P_AUV_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_AUV_T * ) _ssGetModelBlockIO ( S ) ) ;
if ( _rtDW -> TimeStampA == ( rtInf ) ) { _rtDW -> TimeStampA = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA ; } else if ( _rtDW -> TimeStampB == (
rtInf ) ) { _rtDW -> TimeStampB = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB ; } else if ( _rtDW -> TimeStampA < _rtDW -> TimeStampB ) {
_rtDW -> TimeStampA = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA ; } else
{ _rtDW -> TimeStampB = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB ; } (
* lastU ) [ 0 ] = _rtB -> B_5_5_0 [ 0 ] ; ( * lastU ) [ 1 ] = _rtB -> B_5_5_0
[ 1 ] ; ( * lastU ) [ 2 ] = _rtB -> B_5_5_0 [ 2 ] ; if ( _rtDW ->
TimeStampA_m == ( rtInf ) ) { _rtDW -> TimeStampA_m = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_e ; } else if ( _rtDW -> TimeStampB_o == ( rtInf ) )
{ _rtDW -> TimeStampB_o = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e ;
} else if ( _rtDW -> TimeStampA_m < _rtDW -> TimeStampB_o ) { _rtDW ->
TimeStampA_m = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_e ; } else {
_rtDW -> TimeStampB_o = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e ; }
( * lastU ) [ 0 ] = _rtB -> B_4_0_1 [ 0 ] ; ( * lastU ) [ 1 ] = _rtB ->
B_4_0_1 [ 1 ] ; ( * lastU ) [ 2 ] = _rtB -> B_4_0_1 [ 2 ] ; if ( _rtDW ->
TimeStampA_l == ( rtInf ) ) { _rtDW -> TimeStampA_l = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_b ; } else if ( _rtDW -> TimeStampB_n == ( rtInf ) )
{ _rtDW -> TimeStampB_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_j ;
} else if ( _rtDW -> TimeStampA_l < _rtDW -> TimeStampB_n ) { _rtDW ->
TimeStampA_l = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_b ; } else {
_rtDW -> TimeStampB_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_j ; }
( * lastU ) [ 0 ] = _rtB -> B_5_14_0 [ 0 ] ; ( * lastU ) [ 1 ] = _rtB ->
B_5_14_0 [ 1 ] ; ( * lastU ) [ 2 ] = _rtB -> B_5_14_0 [ 2 ] ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK .
TUbufferPtrs [ 4 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK . Head [ 0 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 0 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 0 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 0 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 0 ] == _rtDW -> TransportDelay_IWORK . Tail [ 0
] ) { if ( ! AUV_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 0 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 0 ] , & _rtDW -> TransportDelay_IWORK . Head [
0 ] , & _rtDW -> TransportDelay_IWORK . Last [ 0 ] , simTime - _rtP -> P_11 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 0 ] ] = simTime ; ( * uBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 0 ] ] = _rtB -> B_3_0_1 [ 0 ] ; _rtDW ->
TransportDelay_IWORK . Head [ 1 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 1 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 1 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 1 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 1 ] == _rtDW -> TransportDelay_IWORK . Tail [ 1
] ) { if ( ! AUV_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 1 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 1 ] , & _rtDW -> TransportDelay_IWORK . Head [
1 ] , & _rtDW -> TransportDelay_IWORK . Last [ 1 ] , simTime - _rtP -> P_11 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 1 ] ] = simTime ; ( * uBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 1 ] ] = _rtB -> B_3_0_1 [ 1 ] ; _rtDW ->
TransportDelay_IWORK . Head [ 2 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 2 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 2 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 2 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 2 ] == _rtDW -> TransportDelay_IWORK . Tail [ 2
] ) { if ( ! AUV_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 2 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 2 ] , & _rtDW -> TransportDelay_IWORK . Head [
2 ] , & _rtDW -> TransportDelay_IWORK . Last [ 2 ] , simTime - _rtP -> P_11 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 2 ] ] = simTime ; ( * uBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 2 ] ] = _rtB -> B_3_0_1 [ 2 ] ; _rtDW ->
TransportDelay_IWORK . Head [ 3 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 3 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 3 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 3 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 3 ] == _rtDW -> TransportDelay_IWORK . Tail [ 3
] ) { if ( ! AUV_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 3 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 3 ] , & _rtDW -> TransportDelay_IWORK . Head [
3 ] , & _rtDW -> TransportDelay_IWORK . Last [ 3 ] , simTime - _rtP -> P_11 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK . Head [ 3 ] ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK . Head [ 3 ] ] = _rtB -> B_3_0_1 [ 3 ] ; } if ( _rtDW ->
TimeStampA_k == ( rtInf ) ) { _rtDW -> TimeStampA_k = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_b5 ; } else if ( _rtDW -> TimeStampB_p == ( rtInf ) )
{ _rtDW -> TimeStampB_p = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_g ;
} else if ( _rtDW -> TimeStampA_k < _rtDW -> TimeStampB_p ) { _rtDW ->
TimeStampA_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_b5 ; } else {
_rtDW -> TimeStampB_p = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_g ; }
( * lastU ) [ 0 ] = _rtB -> B_1_2_0 [ 0 ] ; ( * lastU ) [ 1 ] = _rtB ->
B_1_2_0 [ 1 ] ; ( * lastU ) [ 2 ] = _rtB -> B_1_2_0 [ 2 ] ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> NextOutput =
rt_nrand_Upu32_Yd_f_pw_snf ( & _rtDW -> RandSeed ) * _rtP -> P_17 + _rtP ->
P_16 ; } UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID2 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { B_AUV_T * _rtB ; XDot_AUV_T *
_rtXdot ; _rtXdot = ( ( XDot_AUV_T * ) ssGetdX ( S ) ) ; _rtB = ( ( B_AUV_T *
) _ssGetModelBlockIO ( S ) ) ; _rtXdot -> Integrator1_CSTATE [ 0 ] = _rtB ->
B_5_13_0 [ 0 ] ; _rtXdot -> Integrator_CSTATE [ 0 ] = _rtB -> B_5_5_0 [ 0 ] ;
_rtXdot -> Integrator_CSTATE_m [ 0 ] = _rtB -> B_0_0_5 [ 0 ] ; _rtXdot ->
Integrator1_CSTATE [ 1 ] = _rtB -> B_5_13_0 [ 1 ] ; _rtXdot ->
Integrator_CSTATE [ 1 ] = _rtB -> B_5_5_0 [ 1 ] ; _rtXdot ->
Integrator_CSTATE_m [ 1 ] = _rtB -> B_0_0_5 [ 1 ] ; _rtXdot ->
Integrator1_CSTATE [ 2 ] = _rtB -> B_5_13_0 [ 2 ] ; _rtXdot ->
Integrator_CSTATE [ 2 ] = _rtB -> B_5_5_0 [ 2 ] ; _rtXdot ->
Integrator_CSTATE_m [ 2 ] = _rtB -> B_0_0_5 [ 2 ] ; _rtXdot ->
Integrator_CSTATE_p = _rtB -> B_2_0_1 ; } static void mdlInitializeSizes (
SimStruct * S ) { ssSetChecksumVal ( S , 0 , 2083992262U ) ; ssSetChecksumVal
( S , 1 , 1382837349U ) ; ssSetChecksumVal ( S , 2 , 522903304U ) ;
ssSetChecksumVal ( S , 3 , 2611902542U ) ; { mxArray * slVerStructMat = NULL
; mxArray * slStrMat = mxCreateString ( "simulink" ) ; char slVerChar [ 10 ]
; int status = mexCallMATLAB ( 1 , & slVerStructMat , 1 , & slStrMat , "ver"
) ; if ( status == 0 ) { mxArray * slVerMat = mxGetField ( slVerStructMat , 0
, "Version" ) ; if ( slVerMat == NULL ) { status = 1 ; } else { status =
mxGetString ( slVerMat , slVerChar , 10 ) ; } } mxDestroyArray ( slStrMat ) ;
mxDestroyArray ( slVerStructMat ) ; if ( ( status == 1 ) || ( strcmp (
slVerChar , "10.2" ) != 0 ) ) { return ; } } ssSetOptions ( S ,
SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork ( S ) != sizeof (
DW_AUV_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_AUV_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_AUV_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & AUV_rtDefaultP ) ; rt_InitInfAndNaN ( sizeof ( real_T ) ) ; } static void
mdlInitializeSampleTimes ( SimStruct * S ) { { SimStruct * childS ;
SysOutputFcn * callSysFcns ; childS = ssGetSFunction ( S , 0 ) ; callSysFcns
= ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 1 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 2 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 3 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; } slAccRegPrmChangeFcn ( S , mdlOutputsTID2 ) ; }
static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
