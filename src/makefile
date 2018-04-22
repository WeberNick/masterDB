DIRECTORIES = \
	infra \
	partition \
	segment \
	interpreter \
	threading \
	buffer \
	main \
	\

all:
	for dir in $(DIRECTORIES); do \
		(cd $$dir; $(MAKE) all); \
	done

clean:
	for dir in $(DIRECTORIES); do \
		(cd $$dir; $(MAKE) clean); \
	done
