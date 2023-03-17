build:
	mkdir build
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out
	mv process_generator.out clk.out scheduler.out process.out test_generator.out build

clean:
	rm -r build

all: clean build

run:
	./build/process_generator.out
