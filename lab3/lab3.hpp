#pragma once
#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <cmath>

using namespace std;


void runge_serial();
void runge_parallel();

void mc_serial();
void mc_parallel();
