#include "cam_values.h"

#define SIGMA 0.5
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

int dim = (((int)MAX(3.0, 2*4*SIGMA+1.0)%2==0)? ((int)MAX(3.0, 2*4*SIGMA+1.0)+1):((int)MAX(3.0, 2*4*SIGMA+1.0)));
int dim2 = (int)(MAX(3.0, 2*4*SIGMA+1.0)/2);



unsigned char limit(double a)
{
	if(a > 255)
		return 255;
	else if(a < 0)
		return 0;
	return (unsigned char)a;
}

void gaussian_filter()
{
	int i,j,k,x,y,max = 0,min = 255;
	double mask[dim];
	double sum1, sum2;
	double tmp_buf[MAXHEIGHT][MAXWIDTH];

// 마스크 미리 만들어 두기 . 
	for(i=0; i<dim; i++)
	{
		x = i-dim2;
		mask[i] = exp(-(x*x)/(2*SIGMA*SIGMA)) / (sqrt(2*PI)*SIGMA);
	}


	for(i=0;i<MAXWIDTH;i++)
	{
		for(j=0;j<MAXHEIGHT;j++)
		{
			sum1 = sum2 = 0.0;
			for(k=0; k<dim; k++)
			{
				y = k - dim2 + j;
				if(y>=0 && x<MAXHEIGHT)
				{
					sum1 += mask[k];
					sum2 += (mask[k] * Y_ORG(i,y));
				}
			}
			tmp_buf[j][i] = (sum2/sum1);
		}
	}

	for(j=0; j<MAXHEIGHT; j++)
	{
		for(i=0; i<MAXWIDTH; i++)
		{
			sum1 = sum2 = 0.0;
			for(k=0; k<dim; k++)
			{
				x= k-dim2+i;

				if(x >= 0 && x < MAXWIDTH)
				{
					sum1 += mask[k];
					sum2 += (mask[k] * (tmp_buf[j][x]));
				}
			}	

			img_buf_y[j][i] = (limit(sum2/sum1) < 170 ? 0 : limit(sum2/sum1));
		}
	}
}

