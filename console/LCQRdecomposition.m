//
//  LCQRdecomposition.m
//  Lithium Console
//
//  Created by Liam Elliott on 12/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCQRdecomposition.h"
#import <math.h>


@implementation LCQRdecomposition
-(id)init
{
	[super init];
	
	return self;
}

-(LCQRdecomposition *)initWithMatrix:(LCGeneralMatrix *)matrix
{
	//Initialise
	QR = (double **) [matrix arrayCopy];
	m = [matrix rowDimension];
	n = [matrix colDimension];
	
	int i,j,k;
	
	//main loop
	for (k = 0; k < n; k++)
	{
		double nrm =0;
		for (i = k; i < m; i++)
		{
			//Calculate Hypotaneous
			nrm = [self findHypotBetween:nrm and:QR[i][k]];
		}
		
		if (nrm != 0.0)
		{
			//Form k-th Householder Vector
			if (QR[k][k] < 0)
			{
				nrm = - nrm;
			}
			for (i = k; i < m; i++)
			{
				QR[i][k] /= nrm;
			}
			QR[k][k] += 1.0;
			
			//Apply transformation to remaining columns
			for (j = k + 1; j < n; j++)
			{
				double s = 0.0;
				for (i = k; i < m; i++)
				{
					s += QR[i][k] * QR[i][j];
				}
				s = (-s) / QR[k][k];
				for (i = k; i < m; i++)
				{
					QR[i][j] += s * QR[i][k];
				}
			}
		}
		Rdiag[k] = -nrm;
	}
	return self;
}


-(double) findHypotBetween:(double)a and:(double)b
{
	double r;
	
	if (fabs(a) > fabs(b))
	{
		r = b / a;
		r = fabs(a) * sqrt(1 + r * r);
	}
	else if (b!= 0)
	{
		r = a / b;
		r = fabs(b) * sqrt(1+r*r); 
	}
	else
	{
		r = 0.0;
	}
	return r;
}	

-(void) dealloc
{
	int i;
	for (i =0; i < m; i++)
	{
		QR[i] = nil;
	}
	
	QR    = nil;
	Rdiag = nil;
	
	[super dealloc];
}

-(BOOL) fullRank
{
	int j;
	for (j=0; j<n; j++)
	{
		if (Rdiag[j] == 0)
			return false;
	}
	return true;
}

//return the Householder vectors
//returns: Lower trapezoidal matrix whose columns define the reflections
-(LCGeneralMatrix *) H
{
	LCGeneralMatrix* X = [LCGeneralMatrix matrixWithRowSize:m andColSize:n];
	
	int i, j;
	
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i >= j)
			{
				[X setElementAtRow:i andCol:j toValue:QR[i][j]];
			}
			else
			{
				[X setElementAtRow:i andCol:j toValue:0.0];
			}
		}
	}
	return X;
}

//Return the upper triangular factor
-(LCGeneralMatrix *)R
{
	LCGeneralMatrix* X = [LCGeneralMatrix matrixWithRowSize:n andColSize:n];
	
	int i,j;
	for (i= 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i < j)
			{
				[X setElementAtRow:i andCol:j toValue:QR[i][j]];
			}
			else if (i == j)
			{
				[X setElementAtRow:i andCol:j toValue:Rdiag[i]];
			}
			else
			{
				[X setElementAtRow:i andCol:j toValue:0.0];
			}
		}
	}
	return X;
}

//Generate and Return the (economy-sized) orthogonal factor
-(LCGeneralMatrix *) Q
{
	LCGeneralMatrix *X = [LCGeneralMatrix matrixWithRowSize:m andColSize:n];
	
	int i, j, k;
	
	for (k= n - 1; k >= 0; k--)
	{
		for (i = 0; i < m; i++)
		{
			[X setElementAtRow:i andCol:k toValue:0.0];
		}
		
		[X setElementAtRow:k andCol:k toValue:1.0];
		
		for (j = k; j < n; j++)
		{
			if (QR[k][k] != 0)
			{
				double s = 0.0;
				for (i = k; i <  m; i++)
				{
					s += QR[i][k] * [X getElementForRow:i andCol:j];
				}
				s = (-s) / QR[k][k];
				for (i = k; i < m; i++)
				{
					double temp = [X getElementForRow:i andCol:j];
					[X setElementAtRow:i andCol:j toValue:temp +(s*QR[i][k])];
				}
			}
		}
	}
	return X;
}

//Least Squares solution of A*X =B
// Parameters: B = A matrix with as many rows as A ans any number of columns
//
//Returns: X that minimises the two norm of Q*R*X-b.
//
//Exceptions: Matrix row Dimensions must agree
//			  Matrix is rank deficient

-(LCGeneralMatrix *) solveForMatrix:(LCGeneralMatrix *)matrix
{
	if ([matrix rowDimension] != m)
	{
		return nil;
	}
	if (![self fullRank])
	{
		return nil;
	}
	
	// Copy right hand side
	int nx = [matrix colDimension];
	double** X = (double **) [matrix arrayCopy];
	
	//Compute Y = transpose(Q) *B
	int i,j,k;
	for (k = 0; k < n; k++)
	{
		for (j = 0; j < nx; j++)
		{
			double s = 0.0;
			for (i = k; i < m; i++)
			{
				s += QR[i][k] * X[i][j];
			}
			s = (-s) / QR[k][k];
			for (i = k; i < m; i++)
			{
				X[i][j] += s * QR[i][k];
			}
		}
	}
	
	//Solve R*X = Y
	for (k = n - 1; k >= 0; k--)
	{
		for (j = 0 ; j < nx; j++)
		{
			X[k][j] /= Rdiag[k];
		}
		for (i = 0; i < k; i++)
		{
			for (j=0; j < nx; j++)
			{
				X[i][j] -= X[k][j] * QR[i][k];
			}
		}
	}
	
	LCGeneralMatrix *rMatrix = [LCGeneralMatrix matrixWithData:(double *)X withRowSize:n andColSize:nx];
	return [rMatrix getMatrixWithInitialRow:0 andFinalRow:n-1 initialColumn:0 finalColumn:nx-1];
}
@synthesize QR;
@synthesize Rdiag;
@end