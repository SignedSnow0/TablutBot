generate:
	cmake -S . -B bin/ -DCMAKE_EXPORT_COMPILE_COMMANDS=true -DUSE_LOGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3"
build: generate
	cmake --build bin/ --config Release
lsp: generate
	cp bin/compile_commands.json .
white: build
	./bin/TablutBot WHITE 10 127.0.0.1
black: build
	./bin/TablutBot BLACK 10 127.0.0.1
