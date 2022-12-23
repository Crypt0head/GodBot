build:
	g++ -fdiagnostics-color=always -std=c++20 -g src/main.cpp src/GodBot.cpp src/binance_api.cpp src/base_api.cpp src/Kline.cpp src/curl_object.cpp src/GB_LogData.cpp src/GB_Logger.cpp src/GB_SimpleStrategy.cpp -o cfg/App.out -lcurl -lssl -lcrypto -lpthread -lboost_program_options

run:
	./bin/App.out -c ../cfg/local_config.json -s ../cfg/secrets.json