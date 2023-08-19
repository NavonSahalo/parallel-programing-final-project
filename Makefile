build:
	g++ -fopenmp -c main.c -o main.o
	g++ -fopenmp -c fileReader.c -o fileReader.o
	/usr/local/cuda/bin/nvcc -I./inc -c cudaConv.cu -o cudaConv.o
	g++ -fopenmp -o mainExe  main.o fileReader.o cudaConv.o  /usr/local/cuda/lib64/libcudart_static.a -ldl -lrt

clean:
	rm -f *.o ./mainExe

run:
	./mainExe

