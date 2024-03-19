CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean
all: reverse_byte reverse_block run

reverse_byte: reverse.c
	$(CC) $(CFLAGS) -o reverse_byte reverse.c -D REVERSE_BYTE

reverse_block: reverse.c
	$(CC) $(CFLAGS) -o reverse_block reverse.c -D REVERSE_BLOCK
clean:
	rm -rf reverse_both reverse_block reverse_byte reverse.o input.txt output.txt block.txt byte.txt reverse_block.dSYM reverse_byte.dSYM

run: input.txt
	./reverse_byte input.txt byte.txt
	./reverse_block input.txt block.txt

input.txt:
	 echo "Litwo! Ojczyzno moja! ty jestes jak zdrowie.\
	 \nIle cie trzeba cenic, ten tylko sie dowie,\
	 \nKto cie stracil. Dzis pieknosc twa w calej ozdobie\
	 \nWidze i opisuje, bo tesknie po tobie.\
	 \n\
	 \nPanno Swieta, co Jasnej bronisz Czestochowy\
	 \nI w Ostrej swiecisz Bramie! Ty, co grod zamkowy\
	 \nNowogrodzki ochraniasz z jego wiernym ludem!\
	 \nJak mnie dziecko do zdrowia powrocilas cudem\
	 \n(Gdy od placzacej matki pod Twoje opieke\
	 \nOfiarowany, martwa podnioslem powieke\
	 \nI zaraz moglem pieszo do Twych swiatyn progu\
	 \nIsc za wrocone zycie podziekowac Bogu),\
	 \nTak nas powrocisz cudem na Ojczyzny lono.\
	 \nTymczasem przenos moje dusze uteskniona\
	 \nDo tych pagorkow lesnych, do tych lak zielonych,\
	 \nSzeroko nad blekitnym Niemnem rozciagnionych;\
	 \nDo tych pol malowanych zbozem rozmaitem,\
	 \nWyzlacanych pszenica, posrebrzanych zytem;\
	 \nGdzie bursztynowy swierzop, gryka jak snieg biala,\
	 \nGdzie panienskim rumiencem dziecielina pala,\
	 \nA wszystko przepasane, jakby wstega, miedza\
	 \nZielona, na niej z rzadka ciche grusze siedza." > input.txt
