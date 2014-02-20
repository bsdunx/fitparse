decode: decode.o fit_convert.o fit_crc.o fit.o fit_example.o
encode: encode.o fit.o fit_example.o fit_crc.o
decode.o: decode.c fit_convert.h fit_product.h fit_config.h fit_example.h fit.h
encode.o: encode.c fit_product.h fit_config.h fit_example.h fit.h fit_crc.h
fit.o: fit.c fit_product.h fit_config.h fit_example.h fit.h
fit_convert.o: fit_convert.c fit_convert.h fit_product.h fit_config.h  fit_example.h fit.h fit_crc.h
fit_crc.o: fit_crc.c fit_crc.h fit.h fit_config.h
fit_example.o: fit_example.c fit_product.h fit_config.h fit_example.h fit.h fit_include.h
fit_product.o: fit_product.c fit_product.h fit_config.h fit_example.h fit.h fit_example.c fit_include.h
fit_ram.o: fit_ram.c fit_ram.h fit_product.h fit_config.h fit_example.h fit.h fit_crc.h fit_convert.h

clean:
	-rm -f *.o
	-rm -f decode encode

.PHONY: clean
