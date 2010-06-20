//
//  LCGeneralMatrix.m
//  Lithium Console
//
//  Created by Liam Elliott on 12/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCGeneralMatrix.h"
#import "LCLUdecomposition.h"
#import "LCQRdecomposition.h"


@implementation LCGeneralMatrix

#pragma mark "Initialisation"
+(LCGeneralMatrix *) matrixWithData:(double *)array withRowSize:(int) row andColSize:(int)col
{
	return [[[LCGeneralMatrix alloc] initWithRowSize:row
											 colSize:col 
											 andData:array] autorelease];
}

+(LCGeneralMatrix *) matrixWithRowSize:(int)row andColSize:(int)col
{
	return [[[LCGeneralMatrix alloc] initWithRowSize:row
										  andColSize:col] autorelease];
}

+(LCGeneralMatrix *)identityMatrixWithRowSize:(int)row andColSize:(int)col;
{
	LCGeneralMatrix* A = [LCGeneralMatrix matrixWithRowSize:row andColSize:col];
	
	int i;
	int j;
	
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			if (i == j)
			{
				[A setElementAtRow:i andCol:j toValue:1.0];
			}
			else
			{
				[A setElementAtRow:i andCol:j toValue:0.0];
			}
		}
	}
	return A;
}

-(id) init
{
	[super init];
	return self;
}

-(LCGeneralMatrix *) initWithRowSize:(int)rows andColSize:(int)cols
{
	[self init];
	
	m= rows;
	n = cols;
	
	data = malloc(sizeof(double) *m * n);
	
	int i,j;
	for (i = 0; i<m; i++)
	{
		for (j = 0; j <n;j++)
		{
			data[(i *n) + j] = 0;
			
	//		NSLog(@"initialising value for [%i,%i] to: %f",i,j,data[(i *n) + j]);
		}
	}

	return self;
}

-(LCGeneralMatrix *) initWithRowSize:(int)rows colSize:(int)cols andData:(double *)array
{
	[self initWithRowSize:m
			   andColSize:n];
	
	data = array;
	return self;
}

-(void) dealloc
{
	free(data);
	
	[super dealloc];
}

#pragma mark "Accessors"

-(int)rowDimension
{
	return m;
}

-(int)colDimension
{
	return n;
}

-(double)getElementForRow:(int)row andCol:(int)col
{
	return data[(row * n) +col];
}

-(void)setElementAtRow:(int)row andCol:(int)col toValue:(double)value
{
	data[(row * n) + col] = value;
}

-(double *)array
{
	return data;
}

-(double *)getRow:(int)i
{
	double *temp;
	
	temp = &data[i*n];
	
	return temp;
	
}


#pragma mark "Matrix Operations"
//Multiplication 
-(LCGeneralMatrix *)multiply:(LCGeneralMatrix *)B
{
	int value = [B rowDimension];
	
	if (value != n)
		return nil;
	
	LCGeneralMatrix *X = [LCGeneralMatrix matrixWithRowSize:m andColSize:[B colDimension]];	
	double *Bcolj = malloc(sizeof(double)*n);
	
	int j;
	int k;
	int i;
	
//	NSLog(@"Multplying Matrices Result Size [%i,%i]",[X rowDimension],[X colDimension]);
	for (j = 0; j < [B colDimension]; j++)
	{
		for (k = 0; k < n; k++)
		{	
			Bcolj[k] = [B getElementForRow:k andCol:j];
			//NSLog(@"Bcolj[%i] = %f", k, Bcolj[k]);
		}
		for (i = 0; i < m; i++)
		{
			double* DataRowi = [self getRow:i];
			double s = 0;
			for (k = 0; k < n; k++)
			{
				s += DataRowi[k] * Bcolj[k];
				//NSLog(@"%i: %f * %f = %f", temp++, DataRowi[k], Bcolj[k],s);
			}
//			NSLog(@"Final S = %f", s);
			[X setElementAtRow:i andCol:j toValue:s];
//			NSLog(@"X[%i,%j] = %f",i,j,[X getElementForRow:i andCol:j]);
		}
	}
	
	for (i = 0; i < [X rowDimension]; i++)
	{
		for (j = 0; j<[X colDimension]; j++)
		{
//			NSLog(@"Element[%i,%i] = %f",i,j,[X getElementForRow:i andCol:j]);
		}
	}
	
	free (Bcolj);
	
	return X;
}

-(LCGeneralMatrix *)transpose
{
	LCGeneralMatrix* X = [LCGeneralMatrix matrixWithRowSize:n andColSize:m];
	
	int i;
	int j;
	
	//NSLog(@"Creating Transpose. Dimensions [%i,%i]",[X rowDimension],[X colDimension]);
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			[X setElementAtRow:j andCol:i toValue:[self getElementForRow:i andCol:j]];
		}
	}
		return X;
}



-(double *) arrayCopy
{
	double *C = malloc(sizeof (double) *m*n);
	
	int i;
	for (i = 0; i< m*n; i++)
	{
		C[i] = data[i];
	}
	return C;
}

-(LCGeneralMatrix *)getMatrixUsingArray:(int *)r ofLength:(int)rLength initialColumn:(int)j0 finalColumn:(int) j1
{
	LCGeneralMatrix *X = [LCGeneralMatrix matrixWithRowSize:rLength andColSize:j1-j0+1];
	
	int i;
	int j;
	for (i = 0; i < rLength; i++)
	{
		for (j = j0; j <= j1; j++)
		{
			[X setElementAtRow:i andCol:j-j0 toValue:[self getElementForRow:r[i] andCol:j]];
			//NSLog(@"X[%i][%i] = %f", i, j-j0,[X getElementForRow:i andCol:j-j0]);
		}
	}
	
	return X;
}

-(LCGeneralMatrix *)getMatrixWithInitialRow:(int)i0 andFinalRow:(int)i1 initialColumn:(int)j0  finalColumn:(int)j1
{
	LCGeneralMatrix *X = [LCGeneralMatrix matrixWithRowSize:i1-i0 +1 andColSize:j1-j0+1];
	
	int i;
	int j;
	
	for (i=i0; i <= i1; i++)
	{
		for (j = j0; j <= j1; j++)
		{
			[X setElementAtRow:i-i0 andCol:j-j0 toValue:[self getElementForRow:i andCol:j]];
		}
	}
	
	return X;
}

-(LCGeneralMatrix *)solveForMatrix:(LCGeneralMatrix *) matrix
{
	if (m == n)
	{
		LCLUdecomposition *temp = [[[LCLUdecomposition alloc] initWithMatrix:self] autorelease];
		
		return [temp solveForMatrix:matrix];
	}
	
	LCQRdecomposition *tempQR = [[[LCQRdecomposition alloc] initWithMatrix:self] autorelease];
	return  [tempQR solveForMatrix:matrix];
}

-(LCGeneralMatrix *)inverse
{
	return [self solveForMatrix:[LCGeneralMatrix identityMatrixWithRowSize:m andColSize:m]];
}

-(void) writeDebug
{
	int i,j;
	for (i = 0; i < [self rowDimension]; i++)
	{
		for (j = 0; j < [self colDimension];j++)
		{
//			NSLog(@"Array[%i][%i] = %f",i,j,[self getElementForRow:i
//															andCol:j]);
		}
	}
}

@synthesize m;
@synthesize n;
@synthesize data;
@end
