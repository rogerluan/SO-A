#!/bin/bash

rm testes1.txt;

for i in `seq 0 10`;
do
	CARGAS=$((i * 5))

	for j in `seq 0 $CARGAS`;
	do
		./carga &
	done;

	(echo -ne "$CARGAS cargas | mesmo terminal: " && ./exp2.out) >> testes1.txt


	pkill carga
done