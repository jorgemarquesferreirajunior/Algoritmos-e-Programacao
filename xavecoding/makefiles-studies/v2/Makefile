all: bin ./bin/app

bin:
	mkdir -p bin

./bin/app: ./objfloat_vector.o ./my_timer.o ./apps/main.c
	gcc ./objfloat_vector.o ./my_timer.o ./apps/main.c -I ./include -o ./bin/app
	chmod +x ./bin/app  # Ensure app is executable

run: ./bin/app
	./bin/app

clean:
	rm -rf ./bin/* ./objfloat_vector.o ./my_timer.o

