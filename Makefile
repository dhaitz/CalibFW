SRC=src


# command for documentation generation
DOXYGEN=doxygen


.PHONY: all clean  exe doc

all:
	mkdir -p lib	
	mkdir -p obj
	mkdir -p bin
	$(MAKE) -C $(SRC)

cleanplots:
	rm -rf ./*.png
	rm -rf ./*.pdf
	rm -rf ./*.html

clean:
	$(MAKE) -C $(SRC) clean
	rm -rf lib/*
	rm -rf lib
	rm -rf obj/*
	rm -rf obj
	rm -rf bin/*.exe
exe: 
	$(MAKE) -C $(SRC) exe

progs:
	$(MAKE) -C $(SRC) progs

doc:
	$(DOXYGEN) scripts/Doxygen.cfg

publish:
	rm -fr /disks/ekpwww/web/hauth/public_html/zplusjet/*
	cp -f *.png /disks/ekpwww/web/hauth/public_html/zplusjet/
	cp -f zpj_overview.html /disks/ekpwww/web/hauth/public_html/zplusjet/
	echo "Datatsets copied to public_html"

