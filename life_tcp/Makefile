all: worker master
worker: worker.c
		gcc worker.c -lpthread -o worker
master: master.c
		gcc master.c -lpthread -o master
