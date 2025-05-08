Commands used to  run normal code:
g++ -O3 test_code.cpp autocontrols_utils.cpp -lm -o normalcode
./normalcode
Commands used to run neon code:
g++ -O3 -march=armv8-a+simd -std=c++17 -pg -g neon_opt.cpp -o neontimetest
./neontimetest