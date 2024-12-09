#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#define BENCHMARK_ENABLE_LOG 1

#define MEASURE_TIME(func_call, time_var)                \
    do {                                                \
        unsigned long start_time = millis();            \
        func_call;                                      \
        unsigned long end_time = millis();              \
        time_var = end_time - start_time;               \
        if (BENCHMARK_ENABLE_LOG == 1)                 \
        {                                                   \
            Serial.print(#func_call " took: ");             \
            Serial.print(time_var);                         \
            Serial.println(" ms");                          \
        }                                                   \
    } while (0)

#endif //__BENCHMARK_H__