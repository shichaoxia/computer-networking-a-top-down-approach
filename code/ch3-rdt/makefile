.PHONY: test_abp
test_abp: abp
	./$< <<< $$'100 0.1 0.3 10 1' > test_$<.txt

.PHONY: test_gbn
test_gbn: gbn
	./$< <<< $$"100 0.2 0.2 10 1" > test_$<.txt

.PHONY: clean
clean:
	rm abp gbn *.txt

abp: abp.c main.c rdt.c
	gcc -o $@ $^

gbn: gbn.c main.c rdt.c
	gcc -o $@ $^
