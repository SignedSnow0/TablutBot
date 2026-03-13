generate:
	cmake -S . -B bin/ -DCMAKE_EXPORT_COMPILE_COMMANDS=true
build: generate
	cmake --build bin/
lsp: generate
	cp bin/compile_commands.json .
white: build
	./bin/TablutBot WHITE 127.0.0.1 60
black: build
	./bin/TablutBot BLACK 127.0.0.1 60
