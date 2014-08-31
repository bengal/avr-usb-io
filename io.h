#ifndef __IO_H_
#define __IO_H_

/* concatenation functions */
#define CAT2(a,b)	CAT2EXP(a, b)
#define CAT2EXP(a,b)	a ## b
#define CAT3(a,b,c)	CAT3EXP(a, b, c)
#define CAT3EXP(a,b,c)	a ## b ## c

/*
 * Input module constants
 */
#define IM_PORT		D
#define IM_DDR		CAT2(DDR,IM_PORT)
#define IM_CP_BIT	0
#define IM_PL_BIT	1
#define IM_Q7_BIT	4
#define IM_OUT		CAT2(PORT,IM_PORT)
#define IM_IN		CAT2(PIN,IM_PORT)

/*
 * Output module constants
 */
#define OM_PORT		B
#define OM_DDR		CAT2(DDR,OM_PORT)
/* Pin address signals */
#define OM_S0_BIT	0
#define OM_S1_BIT	1
#define OM_S2_BIT	2
#define OM_S3_BIT	3
/* Mux enable signals */
#define OM_E0_BIT	4
#define OM_E1_BIT	5
#define OM_E2_BIT	6
#define OM_OUT		CAT2(PORT,OM_PORT)
#define OM_IN		CAT2(PIN,OM_PORT)

#endif /* __IO_H_ */
