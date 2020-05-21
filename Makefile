#
CINCLUDE = -I ~/.local/include/pbc -L ~/.local/lib -Wl,-rpath ~/.local/lib 
LIBS = -lpbc -lgmp
CFLAGS = -w -g $(CINCLUDE)

all: csd csd2 interval int_set rand_csd rand_csd_mul rand_csd_mul_new rand_int rand_rev new_csd

interval: interval.c
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

interval_set: interval_set.c
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

csd : csd.c
	gcc $(CFLAGS) -o $@ $^ $(LIBS) -g

csd2 : csd2.c
	g++ $(CFLAGS) -o $@ $^ $(LIBS)

new_csd : new_csd.c
	g++ $(CFLAGS) -o $@ $^ $(LIBS) -g

rand_csd : rand_csd.cc
	g++ $(CFLAGS) -o $@ $^ $(LIBS)

rand_csd_mul : rand_csd_mul.cc
	g++ $(CFLAGS) -o $@ $^ $(LIBS) -std=c++11

rand_csd_mul_new : rand_csd_mul_new.cc
	g++ $(CFLAGS) -o $@ $^ $(LIBS) -std=c++11

rand_rev : rand_rev.cc
	g++ $(CFLAGS) -o $@ $^ $(LIBS)

int_set : int_set.cc
	g++ $(CFLAGS) -o $@ $^ $(LIBS)

rand_int : rand_int.cc
	g++ $(CFLAGS) -o $@ $^ $(LIBS)

sd_set : csd_set.cc
	g++ $(CFAGS) -o $@ $^ $(LIBS) -std=c++11

csd_set : csd_set.cc
	g++ $(CFAGS) -o $@ $^ $(LIBS) -D COMBI -std=c++11

clean: 
	rm ecc_test csd

# ./pbc_test < < /Users/hoh/Downloads/pbc-0.5.14/param/a.param
