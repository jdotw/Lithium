//
//  LCLUdecomposition.m
//  Lithium Console
//
//  Created by Liam Elliott on 12/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//


#import "LCLUdecomposition.h"
#import "LCGeneralMatrix.h"


@implementation LCLUdecomposition

-(id)init
{
	[super init];
	
	return self;
}

-(LCLUdecomposition *)initWithMatrix:(LCGeneralMatrix *)matrix
{
	[self init];
	
	m = [matrix rowDimension];
	n = [matrix colDimension];

	// COPY ARRAY
	LU = malloc(sizeof (double) *m*n);
	
	int i, j,k;
	
	for (i = 0; i<m*n; i++)
	{
		double *temp = [matrix array];
		LU[i] = temp[i];
	}
	
	piv = malloc(sizeof(int) * m);
	
	for (i=0; i< m;i++)
	{
		piv[i] = i;
	}
	pivsign = 1;
	
	
	double* LUrowi;
	double* LUcolj = malloc(sizeof(double) * m);
	
	for (j=0; j <n; j++)
	{
		//Make a copy of the j-th column to localise references.
		for (i =0; i<m; i++)
		{
			LUcolj[i] = LU[(i*n) +j];
		}
		
		//Apply previous transformations
		
		for (i=0; i<m; i++)
		{
			LUrowi =[self getRow:i];
			
			// Most of the time is spent in the following dot product.
			int kmax;
			
			if (i <= j)
			{
				kmax = i;
			}
			else
			{
				kmax = j;
			}
			double s = 0.0;
			
			for (k=0; k <kmax; k++)
			{
				s+= LUrowi[k] * LUcolj[k];
			}
			
			LUrowi[j] = LUcolj[i] -= s;
		}
		
		// Find Pivot and exchange if necessary.
		
		int p = j;
		for (i = j + 1; i < m; i++)
		{
			if (fabs(LUcolj[i]) > fabs(LUcolj[p]))
			{
				p = i;
			}
		}
		if (p != j)
		{
			for (k=0; k <n; k++)
			{
				double t = [self getElementAtRow:p andCol:k];
				[self setElementAtRow:p andCol:k toValue:[self getElementAtRow:j andCol:k]];
				[self setElementAtRow:j andCol:k toValue:t];
			}
			int k2 = piv[p];
			piv[p] = piv[j];
			piv[j] = k2;
			
			pivsign = -pivsign;
		}
		
		// Compute multipliers
		if (j <m & [self getElementAtRow:j andCol:j] != 0.0)
		{
			for (i = j + 1; i < m; i++)
			{
				double t = [self getElementAtRow:i andCol:j];
				t /= [self getElementAtRow:j andCol:j];
				[self setElementAtRow:i andCol:j toValue:t];
			}		
		}
	}
	
	free (LUcolj);
	
	return self;
}

-(void) dealloc
{
	free(LU);
	free(piv);
	
	[super dealloc];
}

-(BOOL) isNonSingular
{
	int j;
	for (j = 0; j < n; j++)
	{
		if ([self getElementAtRow:j andCol:j] == 0)
			return false;
	}
	return true;
}

-(double *)getRow:(int)i
{
	double *temp;
	
	temp = &LU[i*n];
	
	return temp;
}

-(double) getElementAtRow:(int)i andCol:(int)j
{
	return LU[(i *n) + j];
}
-(void) setElementAtRow:(int)i andCol:(int)j toValue:(double)val
{
	LU[(i*n)+j] = val;
}

-(LCGeneralMatrix *) L
{
	LCGeneralMatrix *X = [LCGeneralMatrix matrixWithRowSize:m andColSize:n];
	
	int i;
	int j;
	
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i > j)
			{
				[X setElementAtRow:i andCol:j toValue:[self getElementAtRow: i andCol:j]];
			}
			else if (i == j)
			{
				[X setElementAtRow:i andCol:j toValue:1.0];
			}
			else
			{
				[X setElementAtRow:i andCol:j toValue:0.0];
			}
		}
	}
	return X;
}

-(LCGeneralMatrix *) U
{
	LCGeneralMatrix *X = [LCGeneralMatrix matrixWithRowSize:n andColSize:n];
	
	int i;
	int j;
	
	for (i=0; i < n; i++)
	{
		for (j= 0; j < n; j++)
		{
			if (i <= j)
			{
				[X setElementAtRow:i andCol:j toValue:[self getElementAtRow: i andCol:j]];
			}
			else
			{
				[X setElementAtRow:i andCol:j toValue:0.0];
			}
		}
	}
	return X;	
}

//-(int *)pivot
//{
//	int p[m];
//	
//	int i;
//	for (i=0; i < m; i++)
//	{
//		p[i] = piv[i];
//	}
//	return p;
//}
//
//-(double *)doublePivot
//{
//	double vals[m];
//	int i;
//	
//	for (i = 0 ;i < m; i++)
//	{
//		vals[i] = (double)piv[i];
//	}
//	return vals;
//}

-(double)determinant
{
	if (m != n)
	{
		return 0.0;
	}
	
	double d = (double) pivsign;
	
	int j;
	for (j=0; j < n; j++)
	{
		d *= [self getElementAtRow:j andCol:j];
	}
	return d;
}

-(LCGeneralMatrix *)solveForMatrix:(LCGeneralMatrix *)matrix
{
	if ([matrix rowDimension] != m)
	{
		return nil;
	}
	if (![self isNonSingular])
	{
		return nil;
	}
	
	int nx = [matrix colDimension];
	LCGeneralMatrix *Xmat = [matrix getMatrixUsingArray:piv ofLength:m initialColumn:0 finalColumn:nx-1];
	
	//solve L*Y = B(piv,L)
	int i, j, k;
	
	for (k=0; k < n; k++)
	{
		for (i = k + 1; i < n; i++)
		{
			for (j = 0; j < nx; j++)
			{
				double temp = [Xmat getElementForRow:i andCol:j];
				temp -= [Xmat getElementForRow:k andCol:j] * [self getElementAtRow:i andCol:k];
				[Xmat setElementAtRow:i andCol:j toValue:temp];
			}
		}
	}
	
	//solve U*X = Y;
	for (k = n -1; k >= 0; k--)
	{
		for (j = 0; j < nx; j++)
		{
			double temp = [Xmat getElementForRow:k andCol:j];
			temp /= [self getElementAtRow:k andCol:k];
			[Xmat setElementAtRow:k andCol:j toValue:temp];

		}
		for (i = 0; i < k; i++)
		{
			for (j = 0; j < nx; j++)
			{
				double temp = [Xmat getElementForRow:i andCol:j];
				temp -= [Xmat getElementForRow:k andCol:j] * [self getElementAtRow:i andCol:k];
				[Xmat setElementAtRow:i andCol:j toValue:temp];
			}
		}
	}
	
	return Xmat;
}
@synthesize LU;
@synthesize piv;
@end
