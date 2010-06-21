//
//  LCGeneralMatrix.h
//  Lithium Console
//
//  Created by Liam Elliott on 12/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCGeneralMatrix : NSObject {
	
	int m;
	int n;
	
	double *data;
}

+(LCGeneralMatrix *) matrixWithData:(double *)array withRowSize:(int) row andColSize:(int)col;
+(LCGeneralMatrix *) matrixWithRowSize:(int) row andColSize:(int) col;
+(LCGeneralMatrix *)identityMatrixWithRowSize:(int)row andColSize:(int)col;
-(id) init;
-(void) dealloc;
-(LCGeneralMatrix *) initWithRowSize:(int)rows andColSize:(int)cols;
-(LCGeneralMatrix *) initWithRowSize:(int)rows colSize:(int)cols andData:(double *)array;

-(int)rowDimension;
-(int)colDimension;
-(double *) array;
-(double)getElementForRow:(int)row andCol:(int)col;
-(void)setElementAtRow:(int)row andCol:(int)col toValue:(double)value;
-(double *) arrayCopy;

-(LCGeneralMatrix *)multiply:(LCGeneralMatrix *)matrix;
-(LCGeneralMatrix *)transpose;

-(LCGeneralMatrix *)solveForMatrix:(LCGeneralMatrix *)matrix;
-(LCGeneralMatrix *)inverse;

-(LCGeneralMatrix *)getMatrixUsingArray:(int *)r ofLength:(int)rLength initialColumn:(int)j0 finalColumn:(int) j1;
-(LCGeneralMatrix *)getMatrixWithInitialRow:(int)i0 andFinalRow:(int)i1 initialColumn:(int)j0  finalColumn:(int)j1;

-(void)writeDebug;
@property (nonatomic,getter=rowDimension) int m;
@property (nonatomic,getter=colDimension) int n;
@property (nonatomic,getter=array) double *data;
@end
