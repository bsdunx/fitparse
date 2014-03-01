decode: decode.o fit_sdk.o
encode: encode.o fit_sdk.o

decode.o: fit.c fit_sdk.h
	$(CC) -DDECODE -c fit.c -o $@

encode.o: fit.c fit_sdk.h
	$(CC) -DENCODE -c fit.c -o $@

fit_sdk.o: fit_sdk.c fit_sdk.h

clean:
	-rm -f *.o
	-rm -f decode encode

.PHONY: clean
