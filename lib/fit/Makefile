decode: decode.o fit_convert.o fit_crc.o fit.o fit_sdk.o
encode: encode.o fit.o fit_sdk.o fit_crc.o
decode.o: decode.c fit_convert.h fit_config.h fit_sdk.h fit.h
encode.o: encode.c fit_config.h fit_sdk.h fit.h fit_crc.h
fit.o: fit.c fit_config.h fit_sdk.h fit.h
fit_convert.o: fit_convert.c fit_convert.h fit_config.h fit_sdk.h fit.h fit_crc.h
fit_crc.o: fit_crc.c fit_crc.h fit.h fit_config.h
fit_sdk.o: fit_sdk.c fit_config.h fit_sdk.h fit.h

clean:
	-rm -f *.o
	-rm -f decode encode

.PHONY: clean
