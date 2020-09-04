all:
	g++ assign9.cpp machine_code_generator.cpp -o mainfile
run:
	./mainfile
clean:
	rm mainfile machineCode MemoryArray RegisterArray
