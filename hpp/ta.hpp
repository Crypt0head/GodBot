#pragma once

#include <iostream>
#include <vector>

#define TI_REAL double

int ti_ema(int size, TI_REAL const *inputs, int const options, TI_REAL *outputs) {
    const TI_REAL *input = &inputs[0];
    const int period = options;
    TI_REAL *output = &outputs[0];
    if (period < 1) return -1;
    // if (size <= ti_ema_start(options)) return 0;
    const TI_REAL per = 2 / ((TI_REAL)period + 1);
    TI_REAL val = input[0];
    *output++ = val;
    int i;
    for (i = 1; i < size; ++i) {
        val = (input[i]-val) * per + val;
        *output++ = val;
    }
    return 0;
}

double EMA(const int& n, const std::vector<double>& data, const int& pos = 0){
    if(pos != 0)
    {
        const double a=2/(double)(n+1);
        auto ema = EMA(n,data,pos-1);
        auto res = a*data[pos]+(1-a)*ema;
        return res;
    }

    return data[0];
}