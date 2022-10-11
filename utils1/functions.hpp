#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <bits/stdc++.h>

void set_seed(int seed){
    srand(seed);
}

double random_double(){
	return double(rand())/ (RAND_MAX + 1);
}

double random(double min, double max){
    return min + random_double() * (max - min);
}

#endif