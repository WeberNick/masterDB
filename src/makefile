DIRECTORIES = \
	infra \
	partition \
	segment \
	interpreter \
	buffer \
	cli \
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
