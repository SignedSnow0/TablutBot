generate:
	cmake -S . -B bin/ -DCMAKE_EXPORT_COMPILE_COMMANDS=true
build: generate
	cmake --build bin/
lsp: generate
	cp bin/compile_commands.json .
run: build
	./bin/TablutBot localhost 5800 60
