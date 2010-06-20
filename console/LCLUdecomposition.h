//
//  LCLUdecomposition.h
//  Lithium Console
//
//  Created by Liam Elliott on 12/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCGeneralMatrix.h"


@interface LCLUdecomposition : NSObject {
	
	//Array for internal storage of decomposition
	double * LU;
	
	//Row and column dimensions, and pivot sign
	int m, n, pivsign;
	
	//internal storage of pivot vector
	int* piv;
}

-(id)init;

-(LCLUdecomposition *) initWithMatrix:(LCGeneralMatrix *)matrix;

-(void)dealloc;
-(BOOL) isNonSingular;

	//Get the lower triangle factor
-(LCGeneralMatrix *) L;		
	//Get the upper triangle factor
-(LCGeneralMatrix *) U;

//-(int *)pivot;
//-(double *)doublePivot;

-(double)determinant;
-(LCGeneralMatrix *)solveForMatrix:(LCGeneralMatrix *)matrix;

-(double) getElementAtRow:(int)i andCol:(int)j;
-(void) setElementAtRow:(int)i andCol:(int)j toValue:(double)val;
-(double *)getRow:(int)i;


@property double * LU;
@property int* piv;
@end
