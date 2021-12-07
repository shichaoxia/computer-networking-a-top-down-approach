EMACS := emacs
OSASCRIPT := osascript
ORGS := $(wildcard *.org)
HTMLS := $(ORGS:.org=.html)
ADOCS := $(wildcard *.adoc)


index.html: index.adoc
	asciidoctor -r asciidoctor-diagram index.adoc

.PHONY: watch
watch:
	fswatch -o $(ADOCS) | xargs -n1 -I{} make

.PHONY: livereload
livereload:
	browser-sync start --server --files="*.html"

%.html: %.org
	@echo 'generate $@ from $<'
	@$(EMACS) --batch --quick --eval '$(call orgexport,$(abspath $<))'

define orgexport
(progn\
(setq make-backup-files nil)\
(find-file "$1")\
(org-html-export-to-html))
endef
