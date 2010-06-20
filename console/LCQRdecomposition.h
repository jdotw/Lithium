//
//  LCQRdecomposition.h
//  Lithium Console
//
//  Created by Liam Elliott on 12/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCGeneralMatrix.h"


@interface LCQRdecomposition : NSObject {
	// Array for internal storage of decomposition.
	double** QR;
	
	//Row and column dimensions
	int m,n;
	
	//Array for internal storage of diagonal of R.
	double* Rdiag;
}

-(id)init;
-(LCQRdecomposition *) initWithMatrix:(LCGeneralMatrix *)matrix;
-(void)dealloc;

-(BOOL)fullRank;
-(LCGeneralMatrix *)H;
-(LCGeneralMatrix *)R;
-(LCGeneralMatrix *)Q;

-(LCGeneralMatrix *) solveForMatrix:(LCGeneralMatrix *)matrix;

-(double) findHypotBetween:(double)a and:(double)b;
@property double** QR;
@property double* Rdiag;
@end
