#include <stdio.h>

void split1(double num, int *int_part, double *frac_part){
	*int_part = (int)num;
	*frac_part = num - *int_part;
}

int main(void){
	double num, fraction;
	int integer;

	printf("Please enter a real number: ");
    num = 1.4;

    split1(num, &integer, &fraction);
	
	printf("The integer part is %d\n", integer);
	printf("The remaining fraction is %f\n", fraction);

	return 0;
}

