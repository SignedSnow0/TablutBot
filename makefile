generate:
	cmake -S . -B bin/ -DCMAKE_EXPORT_COMPILE_COMMANDS=true -DUSE_LOGS=ON -DCMAKE_BUILD_TYPE=Debug
build: generate
	cmake --build bin/ --config Debug
lsp: generate
	cp bin/compile_commands.json .
white: build
	./bin/TablutBot WHITE 127.0.0.1 10
black: build
	./bin/TablutBot BLACK 127.0.0.1 10
